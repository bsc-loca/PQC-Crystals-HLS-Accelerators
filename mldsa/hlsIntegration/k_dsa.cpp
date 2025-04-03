#include "kernel.hpp"
const unsigned int paramk = K;
const unsigned int paraml = L;
const unsigned int seedbytes = SEEDBYTES;

const unsigned int paramn = NI;
const unsigned int crhbytes = CRHBYTES;
const unsigned int crypto = CRYPTO_BYTES;
const unsigned int step = STEP;

static const int32_t zetas[NI] = {
         0,    25847, -2608894,  -518909,   237124,  -777960,  -876248,   466468,
   1826347,  2353451,  -359251, -2091905,  3119733, -2884855,  3111497,  2680103,
   2725464,  1024112, -1079900,  3585928,  -549488, -1119584,  2619752, -2108549,
  -2118186, -3859737, -1399561, -3277672,  1757237,   -19422,  4010497,   280005,
   2706023,    95776,  3077325,  3530437, -1661693, -3592148, -2537516,  3915439,
  -3861115, -3043716,  3574422, -2867647,  3539968,  -300467,  2348700,  -539299,
  -1699267, -1643818,  3505694, -3821735,  3507263, -2140649, -1600420,  3699596,
    811944,   531354,   954230,  3881043,  3900724, -2556880,  2071892, -2797779,
  -3930395, -1528703, -3677745, -3041255, -1452451,  3475950,  2176455, -1585221,
  -1257611,  1939314, -4083598, -1000202, -3190144, -3157330, -3632928,   126922,
   3412210,  -983419,  2147896,  2715295, -2967645, -3693493,  -411027, -2477047,
   -671102, -1228525,   -22981, -1308169,  -381987,  1349076,  1852771, -1430430,
  -3343383,   264944,   508951,  3097992,    44288, -1100098,   904516,  3958618,
  -3724342,    -8578,  1653064, -3249728,  2389356,  -210977,   759969, -1316856,
    189548, -3553272,  3159746, -1851402, -2409325,  -177440,  1315589,  1341330,
   1285669, -1584928,  -812732, -1439742, -3019102, -3881060, -3628969,  3839961,
   2091667,  3407706,  2316500,  3817976, -3342478,  2244091, -2446433, -3562462,
    266997,  2434439, -1235728,  3513181, -3520352, -3759364, -1197226, -3193378,
    900702,  1859098,   909542,   819034,   495491, -1613174,   -43260,  -522500,
   -655327, -3122442,  2031748,  3207046, -3556995,  -525098,  -768622, -3595838,
    342297,   286988, -2437823,  4108315,  3437287, -3342277,  1735879,   203044,
   2842341,  2691481, -2590150,  1265009,  4055324,  1247620,  2486353,  1595974,
  -3767016,  1250494,  2635921, -3548272, -2994039,  1869119,  1903435, -1050970,
  -1333058,  1237275, -3318210, -1430225,  -451100,  1312455,  3306115, -1962642,
  -1279661,  1917081, -2546312, -1374803,  1500165,   777191,  2235880,  3406031,
   -542412, -2831860, -1671176, -1846953, -2584293, -3724270,   594136, -3776993,
  -2013608,  2432395,  2454455,  -164721,  1957272,  3369112,   185531, -1207385,
  -3183426,   162844,  1616392,  3014001,   810149,  1652634, -3694233, -1799107,
  -3038916,  3523897,  3866901,   269760,  2213111,  -975884,  1717735,   472078,
   -426683,  1723600, -1803090,  1910376, -1667432, -1104333,  -260646, -3833893,
  -2939036, -2235985,  -420899, -2286327,   183443,  -976891,  1612842, -3545687,
   -554416,  3919660,   -48306, -1362209,  3937738,  1400424,  -846154,  1976782
};

const uint64_t KeccakF_RoundConstants[NROUNDS] = {
  (uint64_t)0x0000000000000001ULL,
  (uint64_t)0x0000000000008082ULL,
  (uint64_t)0x800000000000808aULL,
  (uint64_t)0x8000000080008000ULL,
  (uint64_t)0x000000000000808bULL,
  (uint64_t)0x0000000080000001ULL,
  (uint64_t)0x8000000080008081ULL,
  (uint64_t)0x8000000000008009ULL,
  (uint64_t)0x000000000000008aULL,
  (uint64_t)0x0000000000000088ULL,
  (uint64_t)0x0000000080008009ULL,
  (uint64_t)0x000000008000000aULL,
  (uint64_t)0x000000008000808bULL,
  (uint64_t)0x800000000000008bULL,
  (uint64_t)0x8000000000008089ULL,
  (uint64_t)0x8000000000008003ULL,
  (uint64_t)0x8000000000008002ULL,
  (uint64_t)0x8000000000000080ULL,
  (uint64_t)0x000000000000800aULL,
  (uint64_t)0x800000008000000aULL,
  (uint64_t)0x8000000080008081ULL,
  (uint64_t)0x8000000000008080ULL,
  (uint64_t)0x0000000080000001ULL,
  (uint64_t)0x8000000080008008ULL
};

static void computeVer(uint64_t &A, uint64_t &B, uint64_t &C, uint64_t &Di, uint64_t &E,
                    uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
#pragma HLS INLINE OFF
    A = a ^ ((~b) & c);
    B = b ^ ((~c) & d);
    C = c ^ ((~d) & e);
    Di = d ^ ((~e) & a);
    E = e ^ ((~a) & b);
}

static void myxorVer(uint64_t &A, uint64_t &B, uint64_t &C, uint64_t &Di, uint64_t &E,
                  uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
#pragma HLS INLINE OFF
    A ^= a;
    B ^= b;
    C ^= c;
    Di ^= d;
    E ^= e;
}

static void KeccakF1600_StatePermuteVer(uint64_t state[25])
{
        int round;

        uint64_t Aba, Abe, Abi, Abo, Abu;
        uint64_t Aga, Age, Agi, Ago, Agu;
        uint64_t Aka, Ake, Aki, Ako, Aku;
        uint64_t Ama, Ame, Ami, Amo, Amu;
        uint64_t Asa, Ase, Asi, Aso, Asu;
        uint64_t BCa, BCe, BCi, BCo, BCu;
        uint64_t Da, De, Di, Do, Du;
        uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
        uint64_t Ega, Ege, Egi, Ego, Egu;
        uint64_t Eka, Eke, Eki, Eko, Eku;
        uint64_t Ema, Eme, Emi, Emo, Emu;
        uint64_t Esa, Ese, Esi, Eso, Esu;

        //copyFromState(A, state)
        Aba = state[ 0];
        Abe = state[ 1];
        Abi = state[ 2];
        Abo = state[ 3];
        Abu = state[ 4];
        Aga = state[ 5];
        Age = state[ 6];
        Agi = state[ 7];
        Ago = state[ 8];
        Agu = state[ 9];
        Aka = state[10];
        Ake = state[11];
        Aki = state[12];
        Ako = state[13];
        Aku = state[14];
        Ama = state[15];
        Ame = state[16];
        Ami = state[17];
        Amo = state[18];
        Amu = state[19];
        Asa = state[20];
        Ase = state[21];
        Asi = state[22];
        Aso = state[23];
        Asu = state[24];

permute:
        for(round = 0; round < NROUNDS; round += 1) {
            //    prepareTheta
            BCa = Aba^Aga^Aka^Ama^Asa;
            BCe = Abe^Age^Ake^Ame^Ase;
            BCi = Abi^Agi^Aki^Ami^Asi;
            BCo = Abo^Ago^Ako^Amo^Aso;
            BCu = Abu^Agu^Aku^Amu^Asu;

            //thetaRhoPiChiIotaPrepareTheta(round, A, E)
            Da = BCu^ROL(BCe, 1);
            De = BCa^ROL(BCi, 1);
            Di = BCe^ROL(BCo, 1);
            Do = BCi^ROL(BCu, 1);
            Du = BCo^ROL(BCa, 1);

            myxorVer(Aba, Age, Aki, Amo, Asu, Da, De, Di, Do, Du);
            BCa = Aba;
            BCe = ROL(Age, 44);
            BCi = ROL(Aki, 43);
            BCo = ROL(Amo, 21);
            BCu = ROL(Asu, 14);
            computeVer(Eba, Ebe, Ebi, Ebo, Ebu, BCa, BCe, BCi, BCo, BCu);
            Eba ^= (uint64_t)KeccakF_RoundConstants[round];

            myxorVer(Abo, Agu, Aka, Ame, Asi, Do, Du, Da, De, Di);
            BCa = ROL(Abo, 28);
            BCe = ROL(Agu, 20);
            BCi = ROL(Aka,  3);
            BCo = ROL(Ame, 45);
            BCu = ROL(Asi, 61);
            computeVer(Ega, Ege, Egi, Ego, Egu, BCa, BCe, BCi, BCo, BCu);

            myxorVer(Abe, Agi, Ako, Amu, Asa, De, Di, Do, Du, Da);
            BCa = ROL(Abe,  1);
            BCe = ROL(Agi,  6);
            BCi = ROL(Ako, 25);
            BCo = ROL(Amu,  8);
            BCu = ROL(Asa, 18);
            computeVer(Eka, Eke, Eki, Eko, Eku, BCa, BCe, BCi, BCo, BCu);

            myxorVer(Abu, Aga, Ake, Ami, Aso, Du, Da, De, Di, Do);
            BCa = ROL(Abu, 27);
            BCe = ROL(Aga, 36);
            BCi = ROL(Ake, 10);
            BCo = ROL(Ami, 15);
            BCu = ROL(Aso, 56);
            computeVer(Ema, Eme, Emi, Emo, Emu, BCa, BCe, BCi, BCo, BCu);

            myxorVer(Abi, Ago, Aku, Ama, Ase, Di, Do, Du, Da, De);
            BCa = ROL(Abi, 62);
            BCe = ROL(Ago, 55);
            BCi = ROL(Aku, 39);
            BCo = ROL(Ama, 41);
            BCu = ROL(Ase,  2);
            computeVer(Esa, Ese, Esi, Eso, Esu, BCa, BCe, BCi, BCo, BCu);

            Aba = Eba;
            Abe = Ebe;
            Abi = Ebi;
            Abo = Ebo;
            Abu = Ebu;
            Aga = Ega;
            Age = Ege;
            Agi = Egi;
            Ago = Ego;
            Agu = Egu;
            Aka = Eka;
            Ake = Eke;
            Aki = Eki;
            Ako = Eko;
            Aku = Eku;
            Ama = Ema;
            Ame = Eme;
            Ami = Emi;
            Amo = Emo;
            Amu = Emu;
            Asa = Esa;
            Ase = Ese;
            Asi = Esi;
            Aso = Eso;
            Asu = Esu;
        }

        //copyToState(state, A)
        state[ 0] = Aba;
        state[ 1] = Abe;
        state[ 2] = Abi;
        state[ 3] = Abo;
        state[ 4] = Abu;
        state[ 5] = Aga;
        state[ 6] = Age;
        state[ 7] = Agi;
        state[ 8] = Ago;
        state[ 9] = Agu;
        state[10] = Aka;
        state[11] = Ake;
        state[12] = Aki;
        state[13] = Ako;
        state[14] = Aku;
        state[15] = Ama;
        state[16] = Ame;
        state[17] = Ami;
        state[18] = Amo;
        state[19] = Amu;
        state[20] = Asa;
        state[21] = Ase;
        state[22] = Asi;
        state[23] = Aso;
        state[24] = Asu;
}

template <typename T>
static void readmemVer(hls::stream<T> &out, T *in, int size)
{
readmemVer:
    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II=1
        out << in[i];
    }
}

template <typename T>
static void duplicateVer(hls::stream<T> &out1, hls::stream<T> &out2,
                      hls::stream<T> &in, int size)
{
duplicateVer:
    for (unsigned int i = 0; i < size; i++) {
#pragma HLS PIPELINE II=1
        T t = in.read();
        out1 << t;
        out2 << t;
    }
}

template <typename T>
static void writememVer(T *out, hls::stream<T> &stream)
{
writememVer:
        out[0] = stream.read();

}

template <typename T>
static void printVer(hls::stream<T> &out, hls::stream<T> &in,
                  unsigned int packVern, unsigned int n, char *str)
{
    for (unsigned int p = 0; p < packVern; p++)
        for (unsigned int i = 0; i < n; i++) {
            T t = in.read();
            printVerf("fpga:%s -> [%d] = %d\n", str, i, t);
            out << t;
        }
}

int32_t montgomeryVer_reduceVer(int64_t a)
{
  int32_t t;

  t = (int64_t)(int32_t)a * QINV;
  t = (a - (int64_t)t * Q) >> 32;
  return t;
}

static void unpackVer_pkVer(hls::stream<uint8_t> &s_rho, hls::stream<int32_t> &s_t1,
                      hls::stream<uint8_t> &s_pk)
{
        for (unsigned int i = 0; i < SEEDBYTES; i++) {
#pragma HLS PIPELINE II=1
            s_rho << s_pk.read();
        }
        for (unsigned int i = 0; i < K * NI / 4; i++) {
#pragma HLS PIPELINE II=5
            uint8_t a0 = s_pk.read();
            uint8_t a1 = s_pk.read();
            uint8_t a2 = s_pk.read();
            uint8_t a3 = s_pk.read();
            uint8_t a4 = s_pk.read();

            int32_t t;
            t = (int32_t)((a0 >> 0) | ((uint32_t)a1 << 8)) & 0x3FF;
            s_t1 << t;
            t = (int32_t)((a1 >> 2) | ((uint32_t)a2 << 6)) & 0x3FF;
            s_t1 << t;
            t = (int32_t)((a2 >> 4) | ((uint32_t)a3 << 4)) & 0x3FF;
            s_t1 << t;
            t = (int32_t)((a3 >> 6) | ((uint32_t)a4 << 2)) & 0x3FF;
            s_t1 << t;
        }
}

static void unpackVer_sigVer(hls::stream<int> &s_ver, hls::stream<uint8_t> &s_c,
                       hls::stream<int32_t> &s_z, hls::stream<int32_t> &s_h,
                       hls::stream<uint8_t> &s_sig)
{

        for (unsigned int i = 0; i < CTILDEBYTES; i++) {
            s_c << s_sig.read();
        }
#if GAMMA1 == (1 << 17)
        for(unsigned int i = 0; i < L * NI / 4; i++) {
            uint8_t a0 = s_sig.read();
            uint8_t a1 = s_sig.read();
            uint8_t a2 = s_sig.read();
            uint8_t a3 = s_sig.read();
            uint8_t a4 = s_sig.read();
            uint8_t a5 = s_sig.read();
            uint8_t a6 = s_sig.read();
            uint8_t a7 = s_sig.read();
            uint8_t a8 = s_sig.read();
            int32_t t;

            t  = a0;
            t |= (uint32_t)a1 << 8;
            t |= (uint32_t)a2 << 16;
            t &= 0x3FFFF;
            s_z << GAMMA1 - t;

            t  = a2 >> 2;
            t |= (uint32_t)a3 << 6;
            t |= (uint32_t)a4 << 14;
            t &= 0x3FFFF;
            s_z << GAMMA1 - t;

            t  = a4 >> 4;
            t |= (uint32_t)a5 << 4;
            t |= (uint32_t)a6 << 12;
            t &= 0x3FFFF;
            s_z << GAMMA1 - t;

            t  = a6 >> 6;
            t |= (uint32_t)a7 << 2;
            t |= (uint32_t)a8 << 10;
            t &= 0x3FFFF;
            s_z << GAMMA1 - t;
      }
#elif GAMMA1 == (1 << 19)
        for(unsigned int i = 0; i < L * NI / 2; i++) {
            uint8_t a0 = s_sig.read();
            uint8_t a1 = s_sig.read();
            uint8_t a2 = s_sig.read();
            uint8_t a3 = s_sig.read();
            uint8_t a4 = s_sig.read();
            int32_t t;

            t  = a0;
            t |= (uint32_t)a1 << 8;
            t |= (uint32_t)a2 << 16;
            t &= 0xFFFFF;
            s_z << GAMMA1 - t;

            t  = a2 >> 4;
            t |= (uint32_t)a3 << 4;
            t |= (uint32_t)a4 << 12;
            t &= 0xFFFFF;
            s_z << GAMMA1 - t;
        }
#endif
        int ret = 0;
        uint8_t sig[POLYVECH_PACKEDBYTES];
        for (unsigned int i = 0; i < POLYVECH_PACKEDBYTES; i++) {
            sig[i] = s_sig.read();
        }
        unsigned int k = 0;
        for (unsigned int i = 0; i < K; i++) {
            int32_t h[NI];
            for (unsigned int j = 0; j < NI; j++) {
                h[j] = 0;
            }
            if (sig[OMEGA + i] < k || sig[OMEGA + i] > OMEGA) {
                ret = -1;
            }
            for(unsigned int j = k; j < sig[OMEGA + i]; j++) {
                if (j > k && sig[j] <= sig[j-1]) {
                    ret = -1;
                }
                h[sig[j]] = 1;
            }
            for (unsigned int j = 0; j < NI; j++) {
                s_h << h[j];
            }
            k = sig[OMEGA + i];
        }
        for (unsigned int i = k; i < OMEGA; i++) {
            if (sig[i]) {
                ret = -1;
            }
        }
        s_ver << ret;
}

static void chknormVer(hls::stream<int> &s_ver, hls::stream<int32_t> &s_z, unsigned int times, int32_t bound)
{
    int32_t B = bound;
        int ret = 0;
        if (B > (Q - 1) / 8)
            ret = -1;
        for (unsigned int i = 0; i < times * NI; i++) {
#pragma HLS PIPELINE II=1
            int32_t a = s_z.read();
            int32_t t = a >> 31;
            t = a - (t & 2 * a);
            if (t >= B)
                ret = -1;
        }
        s_ver << ret;
}

/*
 * Begin of keccak
 */
static void keccak_initVer(uint64_t s[25])
{
    for (unsigned int i = 0; i < 25; i++) {
        s[i] = 0;
    }
}

static void absorb_rateVer(uint64_t s[25], hls::stream<uint8_t> &in, unsigned int inlen)
{
    for (unsigned int j = 0; j < inlen; j++) {
        s[j / 8] ^= (uint64_t)in.read() << 8 * (j % 8);
    }
}


static void keccak_absorbVer(uint64_t s[25],
                          unsigned int &pos,
                          unsigned int r,
                          hls::stream<uint8_t> &in,
                          size_t inlen)
{
    unsigned int blocks = inlen / r;
    for (unsigned int i = 0; i < blocks; i++) {
        absorb_rateVer(s, in, r);
        KeccakF1600_StatePermuteVer(s);
    }

    inlen -= blocks * r;
    absorb_rateVer(s, in, inlen);
    pos = inlen;
}

static void keccak_finalizeVer(uint64_t s[25], unsigned int pos, unsigned int r, bool ver)
{
    uint8_t p = 0x1F;
    s[pos / 8] ^= (uint64_t)p << 8 * (pos % 8);
    if (ver)
        s[(r - 1) / 8] ^= 1ULL << 63;
    else
        s[r / 8 - 1] ^= 1ULL << 63;
}

/*
 * We are sure that inlen < r -> no permutes needed
 */
static void make_bufVer_stateVer(uint64_t s[25], hls::stream<uint8_t> &in, bool doit)
{
#pragma HLS INLINE OFF
    if (doit) {
        keccak_initVer(s);
        absorb_rateVer(s, in, SEEDBYTES + 2);
        keccak_finalizeVer(s, SEEDBYTES + 2, SHAKE128_RATE, 0);
    }
}

static void squeeze_outVer(hls::stream<uint8_t> &out, unsigned int outlen,
                        uint64_t s[25], unsigned int &pos)
{
    for (unsigned int i = 0; i < outlen; i++) {
        uint8_t t = s[pos / 8] >> 8 * (pos % 8);
        out << t;
        pos++;
    }
}

/*
 * outlen is SHAKE128_RATE which is a multiple of 3
 */
static void make_bufVer_squeezeVer_outVer(hls::stream<ap_uint<24>> &out, uint64_t s[25])
{
#pragma HLS INLINE OFF
    for (unsigned int i = 0; i < SHAKE128_RATE; i += 3) {
#pragma HLS PIPELINE II=1
        ap_uint<24> r;
        uint8_t t = s[i / 8] >> 8 * (i % 8);
        r(7, 0) = t;
        t = s[(i + 1) / 8] >> 8 * ((i + 1) % 8);
        r(15, 8) = t;
        t = s[(i + 2) / 8] >> 8 * ((i + 2) % 8);
        r(23, 16) = t;
        out << r;
    }
}

static void copy_stateVer(uint64_t dst[25], uint64_t src[25])
{
    for (unsigned int i = 0; i < 25; i++) {
        dst[i] = src[i];
    }
}

/*
 * All calls to squeeze happen after setting pos to r,
 * so we immediately do the permutation
 * This can be seen both in absorb_once and finalize functions
 */
static void keccak_squeezeVer(hls::stream<uint8_t> &out,
                                   size_t outlen,
                                   uint64_t s[25],
                                   unsigned int &pos,
                                   unsigned int r)
{
    uint64_t s2[25];
    KeccakF1600_StatePermuteVer(s);
    copy_stateVer(s2, s);
    pos = 0;

    unsigned int blocks = outlen / r;
    for (unsigned int i = 0; i < blocks; i++) {
        squeeze_outVer(out, r, s2, pos);
        KeccakF1600_StatePermuteVer(s);
        copy_stateVer(s2, s);
        pos = 0;
    }
    outlen -= blocks * r;
    squeeze_outVer(out, outlen, s2, pos);
}

/*
 * outlen = POLY_UNIFORM_NBLOCKS * STREAM128_BLOCKBYTES
 * we know that POLY_UNIFORM_NBLOCKS > 2 so we can use that minus 2 without issues
 */
static void make_bufVer_squeezeVer(hls::stream<ap_uint<24>> &out, hls::stream<uint8_t> &in,
                             uint64_t s[25], uint64_t s2[25], bool doit)
{
    uint64_t s3[25];
#pragma HLS ARRAY_PARTITION variable=s3 complete

    /*
     * case(i)
     *     POLY_UNIFORM_NBLOCKS - 2: absorb next
     *     POLY_UNIFORM_NBLOCKS - 1: permute next
     */
    copy_stateVer(s3, s);
    for (unsigned int i = 0; i < POLY_UNIFORM_NBLOCKS - 2; i++) {
#pragma HLS PIPELINE OFF
        make_bufVer_squeezeVer_outVer(out, s3);
        KeccakF1600_StatePermuteVer(s);
        copy_stateVer(s3, s);
    }
    {
        make_bufVer_stateVer(s2, in, doit);
        make_bufVer_squeezeVer_outVer(out, s3);
        KeccakF1600_StatePermuteVer(s);
        copy_stateVer(s3, s);
    }
    {
        make_bufVer_squeezeVer_outVer(out, s3);
        KeccakF1600_StatePermuteVer(s2);
    }

}

static void keccak_squeezeVer_array(uint8_t *buf, uint64_t s[25], unsigned int rate)
{
#pragma HLS INLINE OFF
    for (unsigned int i = 0; i < rate; i++) {
        buf[i] = s[i / 8] >> 8 * (i % 8);
    }
}
/*
 * End of keccak
 */

static void shakeVer(hls::stream<uint8_t> &out, unsigned int outlen,
                  hls::stream<uint8_t> &in, unsigned int inlen,
                  unsigned int r, bool version)
{
        uint64_t s[25];
        unsigned int pos = 0;
        keccak_initVer(s);
        keccak_absorbVer(s, pos, r, in, inlen);
        keccak_finalizeVer(s, pos, r, version);
        keccak_squeezeVer(out, outlen, s, pos, r);
}

template <typename T>
static void mergeVer_unbalancedVer(hls::stream<T> &out, hls::stream<T> &in1,
                  hls::stream<T> &in2, int first, unsigned int second)
{
mergeVer:
        for (unsigned int i = 0; i < first; i++) {
#pragma HLS PIPELINE II=1
            out << in1.read();
        }
        for (unsigned int i = 0; i < second; i++) {
#pragma HLS PIPELINE II=1
            out << in2.read();
        }
}

static void challengeVer(hls::stream<int32_t> &out, hls::stream<uint8_t> &in,
                      hls::stream<ap_uint<1>> &signal)
{
        uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
        uint8_t buf[SHAKE256_RATE];
        int32_t c[NI];
        unsigned int pos = 0;
        keccak_initVer(s);
        keccak_absorbVer(s, pos, SHAKE256_RATE, in, CTILDEBYTES);
        keccak_finalizeVer(s, pos, SHAKE256_RATE, 0);
        KeccakF1600_StatePermuteVer(s);
        keccak_squeezeVer_array(buf, s, SHAKE256_RATE);

        uint64_t signs = 0;
        for(unsigned int i = 0; i < 8; i++)
            signs |= (uint64_t)buf[i] << 8 * i;
        pos = 8;

        for(unsigned int i = 0; i < NI; i++) {
#pragma HLS UNROLL
            c[i] = 0;
        }
        unsigned int b;
        unsigned int i = NI - TAU;
        for (unsigned int j = 8; j < SHAKE256_RATE; j++) {
            b = buf[j];
            if (i < NI && b <= i) {
                c[i] = c[b];
                c[b] = 1 - 2 * (signs & 1);
                signs >>= 1;
                i++;
            }
        }
        if (i < NI)
            signal << 1;
        else
            signal << 0;
        for (unsigned int i = 0; i < NI; i++) {
#pragma HLS PIPELINE II=1
            out << c[i];
        }
}

//This can be optimized to save SEEDBYTS cycles
static void make_seedVer(hls::stream<uint8_t> &out, hls::stream<uint8_t> &in)
{
        uint8_t rho[SEEDBYTES];
        for (unsigned int k = 0; k < SEEDBYTES; k++) {
            rho[k] = in.read();
        }
        for (unsigned int i = 0; i < K; i++) {
            for (unsigned int j = 0; j < L; j++) {
#pragma HLS PIPELINE OFF
                uint16_t nonce = (i << 8) + j;
                for (unsigned int k = 0; k < SEEDBYTES; k++) {
#pragma HLS PIPELINE II=1
                    out << rho[k];
                }
                uint8_t t = nonce;
                out << t;
                t = nonce >> 8;
                out << t;
            }
        }
}

static void make_bufVer(hls::stream<ap_uint<24>> &out, hls::stream<uint8_t> &in)
{
    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
    uint64_t s2[25];
#pragma HLS ARRAY_PARTITION variable=s2 complete
    make_bufVer_stateVer(s2, in, true);
    KeccakF1600_StatePermuteVer(s2);
    for (unsigned int i = 0; i < K * L; i++) {
#pragma HLS PIPELINE OFF
        copy_stateVer(s, s2);
        make_bufVer_squeezeVer(out, in, s, s2, i != K * L - 1);
    }
}

/*
static void rej_uniformVer(hls::stream<int32_t> &out, unsigned int outlen,
                       hls::stream<uint8_t> &in, unsigned int inlen,
                       uint64_t state[25], hls::stream<ap_uint<1>> &signal)
*/
static void rej_uniformVer(hls::stream<int32_t> &out, hls::stream<ap_uint<24>> &in,
                       hls::stream<ap_uint<1>> &signal)
{
    for (unsigned int i = 0; i < K * L; i++) {
        unsigned int ctr, pos;
        unsigned int buflen = POLY_UNIFORM_NBLOCKS * STREAM128_BLOCKBYTES;
        uint32_t t, t0, t1;

        ctr = pos = 0;
        for (unsigned int j = 0; j < buflen; j += 3) {
#pragma HLS PIPELINE II=1
            ap_uint<24> b = in.read();
            uint8_t b0 = b.range(7,0);
            uint8_t b1 = b.range(15,8);
            uint8_t b2 = b.range(23,16);
            t = b0;
            t |= (uint32_t)b1 << 8;
            t |= (uint32_t)b2 << 16;
            t &= 0x7FFFFF;

            if(ctr < NI && t < Q) {
              out << t;
              ctr++;
            }
        }
        if (ctr < NI)
            signal << 1;
        else
            signal << 0;
    }
}

template <typename T>
static void mergeVer(hls::stream<T> &out, hls::stream<T> &in1,
                  hls::stream<T> &in2, int size, int splitVer)
{
    int counter = 0;
mergeVer:
    for (int i = 0; i < size; i++) {
        T t;
        if (counter < splitVer) {
            t = in1.read();
            counter++;
        }
        else {
            t = in2.read();
            counter++;
            if (counter == (splitVer << 1)) {
                counter = 0;
            }
        }
        out << t;
    }
}

template <typename T>
static void splitVer(hls::stream<T> &out1, hls::stream<T> &out2,
                  hls::stream<T> &in, int size, int splitVer)
{
    int counter = 0;
stream_splitVer:
    for (int i = 0; i < size; i++) {
        T t = in.read();
        if (counter < splitVer) {
            out1 << t;
            counter++;
        }
        else {
            out2 << t;
            counter++;
            if (counter == (splitVer << 1)) {
                counter = 0;
            }
        }
    }
}

static void nttVer_butterflyVer(int32_t *x, int32_t *y, int32_t z)
{
    int64_t pr = (int64_t)z * (*y);
    int32_t tmp = montgomeryVer_reduceVer(pr);
    (*y) = (*x) - tmp;
    (*x) = (*x) + tmp;
}

static void nttVer_layerVer(hls::stream<int32_t> &sink_0, hls::stream<int32_t> &sink_1,
                 hls::stream<int32_t> &src_0, hls::stream<int32_t> &src_1,
                 int32_t start, unsigned int times)
{
nttVer_layerVer:
    unsigned int index = start;
    for (unsigned int i = 0; i < 128 * times; i++) {
        int32_t wing1 = src_0.read();
        int32_t wing2 = src_1.read();

        nttVer_butterflyVer(&wing1, &wing2, zetas[index]);

        if (i % 128 < 64) {
            sink_0 << wing1;
            sink_0 << wing2;
        }
        else {
            sink_1 << wing1;
            sink_1 << wing2;
        }
        index++;
        if (index == 2 * start)
            index = start;
    }
}

template <typename T>
static void nttVer(hls::stream<T> &out, hls::stream<T> &in, unsigned int times)
{
#pragma HLS INLINE
    hls::stream<T> s_8_0("snttVer_l_8_0");
    #pragma HLS STREAM variable=s_8_0 depth=128
    hls::stream<T> s_8_1("snttVer_l_8_1");
    #pragma HLS STREAM variable=s_8_1 depth=128
    hls::stream<T> s_7_0("snttVer_l_7_0");
    #pragma HLS STREAM variable=s_7_0 depth=128
    hls::stream<T> s_7_1("snttVer_l_7_1");
    #pragma HLS STREAM variable=s_7_1 depth=128
    hls::stream<T> s_6_0("snttVer_l_6_0");
    #pragma HLS STREAM variable=s_6_0 depth=128
    hls::stream<T> s_6_1("snttVer_l_6_1");
    #pragma HLS STREAM variable=s_6_1 depth=128
    hls::stream<T> s_5_0("snttVer_l_5_0");
    #pragma HLS STREAM variable=s_5_0 depth=128
    hls::stream<T> s_5_1("snttVer_l_5_1");
    #pragma HLS STREAM variable=s_5_1 depth=128
    hls::stream<T> s_4_0("snttVer_l_4_0");
    #pragma HLS STREAM variable=s_4_0 depth=128
    hls::stream<T> s_4_1("snttVer_l_4_1");
    #pragma HLS STREAM variable=s_4_1 depth=128
    hls::stream<T> s_3_0("snttVer_l_3_0");
    #pragma HLS STREAM variable=s_3_0 depth=128
    hls::stream<T> s_3_1("snttVer_l_3_1");
    #pragma HLS STREAM variable=s_3_1 depth=128
    hls::stream<T> s_2_0("snttVer_l_2_0");
    #pragma HLS STREAM variable=s_2_0 depth=128
    hls::stream<T> s_2_1("snttVer_l_2_1");
    #pragma HLS STREAM variable=s_2_1 depth=128
    hls::stream<T> s_1_0("snttVer_l_1_0");
    #pragma HLS STREAM variable=s_1_0 depth=128
    hls::stream<T> s_1_1("snttVer_l_1_1");
    #pragma HLS STREAM variable=s_1_1 depth=128
    hls::stream<T> s_0_0("snttVer_l_0_0");
    #pragma HLS STREAM variable=s_0_0 depth=128
    hls::stream<T> s_0_1("snttVer_l_0_1");
    #pragma HLS STREAM variable=s_0_1 depth=128

    splitVer(s_8_0, s_8_1, in, 256 * times, 128);
    nttVer_layerVer(s_7_0, s_7_1, s_8_0, s_8_1, 1, times);
    nttVer_layerVer(s_6_0, s_6_1, s_7_0, s_7_1, 2, times);
    nttVer_layerVer(s_5_0, s_5_1, s_6_0, s_6_1, 4, times);
    nttVer_layerVer(s_4_0, s_4_1, s_5_0, s_5_1, 8, times);
    nttVer_layerVer(s_3_0, s_3_1, s_4_0, s_4_1, 16, times);
    nttVer_layerVer(s_2_0, s_2_1, s_3_0, s_3_1, 32, times);
    nttVer_layerVer(s_1_0, s_1_1, s_2_0, s_2_1, 64, times);
    nttVer_layerVer(s_0_0, s_0_1, s_1_0, s_1_1, 128, times);
    mergeVer(out, s_0_0, s_0_1, 256 * times, 128);
}

static void inv_butterflyVer(int32_t *x, int32_t *y, int32_t z)
{
    int32_t t = *x;
    *x = t + *y;
    *y = t - *y;
    int64_t pr = (int64_t)z * (*y);
    *y = montgomeryVer_reduceVer(pr);
}

template <typename T>
static void invnttVer_layerVer(hls::stream<T> &sink_0, hls::stream<T> &sink_1,
                  hls::stream<T> &src_0, hls::stream<T> &src_1,
                  T start, unsigned int times)
{
    unsigned int index = start;
invnttVer_layerVer:
    for (unsigned int i = 0; i < 128 * times; i++) {
        T wing1, wing2;
        if ( i % 128 < 64) {
            wing1 = src_0.read();
            wing2 = src_0.read();
        }
        else {
            wing1 = src_1.read();
            wing2 = src_1.read();
        }

        inv_butterflyVer(&wing1, &wing2, -zetas[index - 1]);

        sink_0 << wing1;
        sink_1 << wing2;
        index--;
        if (index == start / 2)
            index = start;
    }
}

template <typename T>
static void invnttVer(hls::stream<T> &out, hls::stream<T> &in, unsigned int times)
{
#pragma HLS INLINE
    hls::stream<T> s_0_0("s_0_0");
    #pragma HLS STREAM variable=s_0_0 depth=128
    hls::stream<T> s_0_1("s_0_1");
    #pragma HLS STREAM variable=s_0_1 depth=128
    hls::stream<T> s_1_0("s_1_280");
    #pragma HLS STREAM variable=s_1_0 depth=128
    hls::stream<T> s_1_1("s_1_1");
    #pragma HLS STREAM variable=s_1_1 depth=128
    hls::stream<T> s_2_0("s_2_0");
    #pragma HLS STREAM variable=s_2_0 depth=128
    hls::stream<T> s_2_1("s_2_1");
    #pragma HLS STREAM variable=s_2_1 depth=128
    hls::stream<T> s_3_0("s_3_0");
    #pragma HLS STREAM variable=s_3_0 depth=128
    hls::stream<T> s_3_1("s_3_1");
    #pragma HLS STREAM variable=s_3_1 depth=128
    hls::stream<T> s_4_0("s_4_0");
    #pragma HLS STREAM variable=s_4_0 depth=128
    hls::stream<T> s_4_1("s_4_1");
    #pragma HLS STREAM variable=s_4_1 depth=128
    hls::stream<T> s_5_0("s_5_0");
    #pragma HLS STREAM variable=s_5_0 depth=128
    hls::stream<T> s_5_1("s_5_1");
    #pragma HLS STREAM variable=s_5_1 depth=128
    hls::stream<T> s_6_0("s_6_0");
    #pragma HLS STREAM variable=s_6_0 depth=128
    hls::stream<T> s_6_1("s_6_1");
    #pragma HLS STREAM variable=s_6_1 depth=128
    hls::stream<T> s_7_0("s_7_0");
    #pragma HLS STREAM variable=s_7_0 depth=128
    hls::stream<T> s_7_1("s_7_1");
    #pragma HLS STREAM variable=s_7_1 depth=128
    hls::stream<T> s_8_0("s_8_0");
    #pragma HLS STREAM variable=s_8_0 depth=128
    hls::stream<T> s_8_1("s_8_1");
    #pragma HLS STREAM variable=s_8_1 depth=128

    splitVer(s_0_0, s_0_1, in, 256 * times, 128);
    invnttVer_layerVer(s_1_0, s_1_1, s_0_0, s_0_1, 256, times);
    invnttVer_layerVer(s_2_0, s_2_1, s_1_0, s_1_1, 128, times);
    invnttVer_layerVer(s_3_0, s_3_1, s_2_0, s_2_1, 64, times);
    invnttVer_layerVer(s_4_0, s_4_1, s_3_0, s_3_1, 32, times);
    invnttVer_layerVer(s_5_0, s_5_1, s_4_0, s_4_1, 16, times);
    invnttVer_layerVer(s_6_0, s_6_1, s_5_0, s_5_1, 8, times);
    invnttVer_layerVer(s_7_0, s_7_1, s_6_0, s_6_1, 4, times);
    invnttVer_layerVer(s_8_0, s_8_1, s_7_0, s_7_1, 2, times);
    mergeVer(out, s_8_0, s_8_1, 256 * times, 128);
}


template <typename T>
static void repeatVerlVer(hls::stream<T> &out, hls::stream<T> &in,
                    unsigned int times)
{

        T tmp[NI * L];
        for (unsigned int j = 0; j < NI * L; j++) {
#pragma HLS PIPELINE II=1
            tmp[j] = in.read();
            out << tmp[j];
        }
        for (unsigned int i = 1; i < times; i++) {
            for (unsigned int j = 0; j < NI * L; j++) {
#pragma HLS PIPELINE II=1
                out << tmp[j];
            }
        }

}

template <typename T>
static void repeatVer(hls::stream<T> &out, hls::stream<T> &in,
                   unsigned int times)
{
        T tmp[NI];
        for (unsigned int j = 0; j < NI; j++) {
#pragma HLS PIPELINE II=1
            tmp[j] = in.read();
            out << tmp[j];
        }
        for (unsigned int i = 1; i < times; i++) {
            for (unsigned int j = 0; j < NI; j++) {
#pragma HLS PIPELINE II=1
                out << tmp[j];
            }
        }
}

static void montgomeryVer(hls::stream<int32_t> &out, hls::stream<int32_t> &in1,
                       hls::stream<int32_t> &in2, unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
        int64_t a = (int64_t)in1.read() * in2.read();
        out << montgomeryVer_reduceVer(a);
    }
}

static void montgomeryVer_add(hls::stream<int32_t> &out, hls::stream<int32_t> &in,
                           unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
        int32_t tmp[NI];
        for (unsigned int j = 0; j < L; j++) {
            for (unsigned int k = 0; k < NI; k++) {
#pragma HLS PIPELINE II=1
                int32_t t = in.read();
                if (j == 0) {
                    tmp[k] = t;
                }
                else if (j == L - 1) {
                    out << tmp[k] + t;
                }
                else {
                    tmp[k] += t;
                }
            }
        }
    }
}

static void shiftlVer(hls::stream<int32_t> &out, hls::stream<int32_t> &in,
                  unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
        int32_t a = in.read();
        a <<= D;
        out << a;
    }
}

static void subVer(hls::stream<int32_t> &out, hls::stream<int32_t> &in1,
                hls::stream<int32_t> &in2, unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
        out << in1.read() - in2.read();
    }
}

static void reducefVer(hls::stream<int32_t> &out, hls::stream<int32_t> &in,
                              unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
        const int32_t f = 41978; // mont^2/256
        int32_t a = in.read();
        out << montgomeryVer_reduceVer((int64_t)f * a);
    }
}

static void reduce32Ver(hls::stream<int32_t> &out, hls::stream<int32_t> &in,
                   unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
        const int32_t f = 41978; // mont^2/256
        int32_t a = in.read();
        int64_t b = (int64_t)f * a;
        int32_t t = (a + (1 << 22)) >> 23;
        t = a - t * Q;
        out << t;
    }
}


static void caddqVer(hls::stream<int32_t> &out, hls::stream<int32_t> &in,
                  unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
        int32_t a = in.read();
        a += (a >> 31) & Q;
        out << a;
    }
}

static int32_t decomposeVer(int32_t *a0, int32_t a)
{
    int32_t a1;

    a1  = (a + 127) >> 7;
#if GAMMA2 == (Q-1)/32
    a1  = (a1 * 1025 + (1 << 21)) >> 22;
    a1 &= 15;
#elif GAMMA2 == (Q-1)/88
    a1  = (a1 * 11275 + (1 << 23)) >> 24;
    a1 ^= ((43 - a1) >> 31) & a1;
#endif

    *a0  = a - a1 * 2 * GAMMA2;
    *a0 -= (((Q - 1) / 2 - *a0) >> 31) & Q;
    return a1;
}

static void hintVer(hls::stream<int32_t> &out, hls::stream<int32_t> &in1,
                 hls::stream<int32_t> &in2, unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
        int32_t a0, a1;
        int32_t a = in1.read();
        unsigned int hintVer = in2.read();

        a1 = decomposeVer(&a0, a);
        if (hintVer == 0)
            out << a1;
#if GAMMA2 == (Q-1)/32
        else if (a0 > 0) {
            int32_t t = (a1 + 1) & 15;
            out << t;
        }
        else {
            int32_t t =  (a1 - 1) & 15;
            out << t;
        }
#elif GAMMA2 == (Q-1)/88
        else if(a0 > 0) {
            int32_t t = (a1 == 43) ?  0 : a1 + 1;
            out << t;
        }
        else {
            int32_t t = (a1 ==  0) ? 43 : a1 - 1;
            out << t;
        }
#endif
    }
}

static void packVer(hls::stream<uint8_t> &out, hls::stream<int32_t> &in,
        unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
#if GAMMA2 == (Q-1)/88
        for(unsigned int i = 0; i < NI / 4; ++i) {
#pragma HLS PIPELINE II=4
            int32_t a0 = in.read();
            int32_t a1 = in.read();
            int32_t a2 = in.read();
            int32_t a3 = in.read();
            uint8_t t;
            t  = a0;
            t |= a1 << 6;
            out << t;
            t  = a1 >> 2;
            t |= a2 << 4;
            out << t;
            t  = a2 >> 4;
            t |= a3 << 2;
            out << t;
        }
#elif GAMMA2 == (Q-1)/32
        for(unsigned int i = 0; i < NI / 2; ++i) {
#pragma HLS PIPELINE II=2
            int32_t a0 = in.read();
            int32_t a1 = in.read();
            uint8_t t;
            t = a0 | (a1 << 4);
            out << t;
        }
#endif
    }
}

static void make_verVer(hls::stream<int> &s_ver, hls::stream<int> &s_ver0,
                     hls::stream<int> &s_ver1, hls::stream<uint8_t> &s_c,
                     hls::stream<uint8_t> &s_c2, hls::stream<ap_uint<1>> &signal,
                     hls::stream<ap_uint<1>> &signal2)
{
        int ver0 = s_ver0.read();
        int ver1 = s_ver1.read();
        ap_uint<1> vers = signal.read();
        int vers2 = 0;

        for (unsigned int i = 0; i < K * L; i++) {
            ap_uint<1> s = signal2.read();
            if (s)
                vers2 = 1;
        }
        int ver2 = 0;
        int ret = 0;
        for (unsigned int i = 0; i < CTILDEBYTES; i++) {
            uint8_t c1 = s_c.read();
            uint8_t c2 = s_c2.read();
            if (c1 != c2)
                ver2 = -1;
        }
        if (ver0 || ver1 || ver2) {
            ret = -1;
        }
        else if (vers)
            ret = -2;
        else if (vers2)
            ret = -3;
        s_ver << ret;

}

extern "C" {
void k_verify(int *ver, uint8_t *sig, uint8_t *m, size_t mlen, uint8_t *pk)
{
    /*
     * Stream declaration begin
     */
    static hls::stream<uint8_t> s_pk("s_pk");
#pragma HLS STREAM variable=s_pk depth=200
    static hls::stream<uint8_t> s_pk_0("s_pk_0");
#pragma HLS STREAM variable=s_pk_0 depth=200
    static hls::stream<uint8_t> s_pk_1("s_pk_1");
#pragma HLS STREAM variable=s_pk_1 depth=200
    static hls::stream<uint8_t> s_rho("s_rho");
#pragma HLS STREAM variable=s_rho depth=200
    static hls::stream<int32_t> s_t1("s_t1");
#pragma HLS STREAM variable=s_t1 depth=1000
    static hls::stream<int32_t> s_t1_s("s_t1_s");
#pragma HLS STREAM variable=s_t1_s depth=5000
    static hls::stream<int32_t> s_t1_t("s_t1_t");
#pragma HLS STREAM variable=s_t1_t depth=5000
    static hls::stream<int32_t> s_t1_m("s_t1_m");
#pragma HLS STREAM variable=s_t1_m depth=5000

    static hls::stream<uint8_t> s_sig("s_sig");
#pragma HLS STREAM variable=s_sig depth=1000
    static hls::stream<int>     s_ver_pre_0("s_ver_pre_0");
#pragma HLS STREAM variable=s_ver_pre_0 depth=100
    static hls::stream<uint8_t> s_c("s_c");
#pragma HLS STREAM variable=s_c depth=200
    static hls::stream<uint8_t> s_c_0("s_c_0");
#pragma HLS STREAM variable=s_c_0 depth=200
    static hls::stream<uint8_t> s_c_1("s_c_1");
#pragma HLS STREAM variable=s_c_1 depth=200
    static hls::stream<int32_t> s_z("s_z");
#pragma HLS STREAM variable=s_z depth=5000
    static hls::stream<int32_t> s_h("s_h");
#pragma HLS STREAM variable=s_h depth=5000

    static hls::stream<int32_t> s_z_0("s_z_0");
#pragma HLS STREAM variable=s_z_0 depth=5000
    static hls::stream<int32_t> s_z_1("s_z_1");
#pragma HLS STREAM variable=s_z_1 depth=5000

    static hls::stream<int>     s_ver_pre_1("s_ver_pre_1");
#pragma HLS STREAM variable=s_ver_pre_1 depth=100
    static hls::stream<int>     s_ver("s_ver");
#pragma HLS STREAM variable=s_ver depth=100

    static hls::stream<uint8_t> s_mu_pre("s_mu_pre");
#pragma HLS STREAM variable=s_mu_pre depth=200
    static hls::stream<uint8_t> s_mu_mrg("s_mu_mrg");
#pragma HLS STREAM variable=s_mu_mrg depth=200
    static hls::stream<uint8_t> s_m("s_m");
#pragma HLS STREAM variable=s_m depth=200
    static hls::stream<uint8_t> s_mu("s_mu");
#pragma HLS STREAM variable=s_mu depth=200

    static hls::stream<int32_t> s_cp("s_cp");
#pragma HLS STREAM variable=s_cp depth=200
    static hls::stream<int32_t> s_cp_t("s_cp_t");
#pragma HLS STREAM variable=s_cp_t depth=200
    static hls::stream<int32_t> s_cp_r("s_cp_r");
#pragma HLS STREAM variable=s_cp_r depth=200

    static hls::stream<uint8_t> s_seed("s_seed");
#pragma HLS STREAM variable=s_seed depth=3000
    static hls::stream<int32_t> s_mat("s_mat");
#pragma HLS STREAM variable=s_mat depth=10000

    static hls::stream<int32_t> s_z_t("s_z_t");
#pragma HLS STREAM variable=s_z_t depth=5000
    static hls::stream<int32_t> s_z_r("s_z_r");
#pragma HLS STREAM variable=s_z_r depth=5000
    static hls::stream<int32_t> s_w1_m("s_w1_m");
#pragma HLS STREAM variable=s_w1_m depth=5000
    static hls::stream<int32_t> s_w1("s_w1");
#pragma HLS STREAM variable=s_w1 depth=5000
    static hls::stream<int32_t> s_w1_s("s_w1_s");
#pragma HLS STREAM variable=s_w1_s depth=5000
    static hls::stream<int32_t> s_w1_r("s_w1_r");
#pragma HLS STREAM variable=s_w1_r depth=5000
    static hls::stream<int32_t> s_w1_i("s_w1_i");
#pragma HLS STREAM variable=s_w1_i depth=5000
    static hls::stream<int32_t> s_w1_f("s_w1_f");
#pragma HLS STREAM variable=s_w1_f depth=5000
    static hls::stream<int32_t> s_w1_c("s_w1_c");
#pragma HLS STREAM variable=s_w1_c depth=5000
    static hls::stream<int32_t> s_w1_h("s_w1_h");
#pragma HLS STREAM variable=s_w1_h depth=5000

    static hls::stream<uint8_t> s_buf("s_buf");
#pragma HLS STREAM variable=s_buf depth=5000
    static hls::stream<uint8_t> s_mubuf("s_mubuf");
#pragma HLS STREAM variable=s_mubuf depth=5000
    static hls::stream<uint8_t> s_c2("s_c2");
#pragma HLS STREAM variable=s_c2 depth=200
    static hls::stream<ap_uint<1>> s_signal("s_signal");
#pragma HLS STREAM variable=s_signal depth=20
    static hls::stream<ap_uint<1>> s_signal2("s_signal2");
#pragma HLS STREAM variable=s_signal2 depth=200

    static hls::stream<ap_uint<24>> s_mat_buf("s_mat_buf");
#pragma HLS STREAM variable=s_mat_buf depth=1000

    /*
     * Stream declaration end
     */
#pragma HLS DATAFLOW

    readmemVer(s_pk, pk, CRYPTO_PUBLICKEYBYTES);
    duplicateVer(s_pk_0, s_pk_1, s_pk, CRYPTO_PUBLICKEYBYTES);
    unpackVer_pkVer(s_rho, s_t1, s_pk_0);

    readmemVer(s_sig, sig, CRYPTO_BYTES);
    unpackVer_sigVer(s_ver_pre_0, s_c, s_z, s_h, s_sig);
    duplicateVer(s_z_0, s_z_1, s_z, L * NI);
    duplicateVer(s_c_0, s_c_1, s_c, CTILDEBYTES);

    chknormVer(s_ver_pre_1, s_z_0, L , GAMMA1 - BETA);

    shakeVer(s_mu_pre, CRHBYTES, s_pk_1, CRYPTO_PUBLICKEYBYTES,
          SHAKE256_RATE, 1);

    readmemVer(s_m, m, mlen);
    mergeVer_unbalancedVer(s_mu_mrg, s_mu_pre, s_m, TRBYTES, mlen);
    shakeVer(s_mu, CRHBYTES, s_mu_mrg, TRBYTES + mlen, SHAKE256_RATE, 0);

    challengeVer(s_cp, s_c_0, s_signal);

    make_seedVer(s_seed, s_rho);
    make_bufVer(s_mat_buf, s_seed);
    rej_uniformVer(s_mat, s_mat_buf, s_signal2);

    nttVer(s_z_t, s_z_1, L);
    repeatVerlVer(s_z_r, s_z_t, K);
    montgomeryVer(s_w1_m, s_mat, s_z_r, K * L * NI);
    montgomeryVer_add(s_w1, s_w1_m, K);

    nttVer(s_cp_t, s_cp, 1);
    shiftlVer(s_t1_s, s_t1, K * NI);
    nttVer(s_t1_t, s_t1_s, K);
    repeatVer(s_cp_r, s_cp_t, K);
    montgomeryVer(s_t1_m, s_cp_r, s_t1_t, K * NI);

    subVer(s_w1_s, s_w1, s_t1_m, K * NI);
    reduce32Ver(s_w1_r, s_w1_s, K * NI);
    invnttVer(s_w1_i, s_w1_r, K);
    reducefVer(s_w1_f, s_w1_i, K * NI);

    caddqVer(s_w1_c, s_w1_f, K * NI);
    hintVer(s_w1_h, s_w1_c, s_h, K * NI);
    packVer(s_buf, s_w1_h, K);

    mergeVer_unbalancedVer(s_mubuf, s_mu, s_buf,
                     CRHBYTES, K * POLYW1_PACKEDBYTES);
    shakeVer(s_c2, CTILDEBYTES, s_mubuf, CRHBYTES + K * POLYW1_PACKEDBYTES,
          SHAKE256_RATE, 0);
    make_verVer(s_ver, s_ver_pre_0, s_ver_pre_1, s_c_1, s_c2,
             s_signal, s_signal2);
    writememVer(ver, s_ver);
}
}
/////////////////////////// signature ////////////////////////////////////////////////////
/////////////////////////// signature ////////////////////////////////////////////////////


static void compute(uint64_t &A, uint64_t &B, uint64_t &C, uint64_t &Di, uint64_t &E,
                uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
#pragma HLS INLINE OFF
A = a ^ ((~b) & c);
B = b ^ ((~c) & d);
C = c ^ ((~d) & e);
Di = d ^ ((~e) & a);
E = e ^ ((~a) & b);
}

static void myxor(uint64_t &A, uint64_t &B, uint64_t &C, uint64_t &Di, uint64_t &E,
              uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
#pragma HLS INLINE OFF
A ^= a;
B ^= b;
C ^= c;
Di ^= d;
E ^= e;
}

static void KeccakF1600_StatePermute(uint64_t state[25])
{
#pragma HLS ARRAY_PARTITION variable=state complete
#pragma HLS ALLOCATION function instances=compute limit=5
#pragma HLS ALLOCATION function instances=myxor limit=5
    int round;

    uint64_t Aba, Abe, Abi, Abo, Abu;
    uint64_t Aga, Age, Agi, Ago, Agu;
    uint64_t Aka, Ake, Aki, Ako, Aku;
    uint64_t Ama, Ame, Ami, Amo, Amu;
    uint64_t Asa, Ase, Asi, Aso, Asu;
    uint64_t BCa, BCe, BCi, BCo, BCu;
    uint64_t Da, De, Di, Do, Du;
    uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
    uint64_t Ega, Ege, Egi, Ego, Egu;
    uint64_t Eka, Eke, Eki, Eko, Eku;
    uint64_t Ema, Eme, Emi, Emo, Emu;
    uint64_t Esa, Ese, Esi, Eso, Esu;

    //copyFromState(A, state)
    Aba = state[ 0];
    Abe = state[ 1];
    Abi = state[ 2];
    Abo = state[ 3];
    Abu = state[ 4];
    Aga = state[ 5];
    Age = state[ 6];
    Agi = state[ 7];
    Ago = state[ 8];
    Agu = state[ 9];
    Aka = state[10];
    Ake = state[11];
    Aki = state[12];
    Ako = state[13];
    Aku = state[14];
    Ama = state[15];
    Ame = state[16];
    Ami = state[17];
    Amo = state[18];
    Amu = state[19];
    Asa = state[20];
    Ase = state[21];
    Asi = state[22];
    Aso = state[23];
    Asu = state[24];

permute:
    for(round = 0; round < NROUNDS; round += 1) {
#pragma HLS PIPELINE II=1
        //    prepareTheta
        BCa = Aba^Aga^Aka^Ama^Asa;
        BCe = Abe^Age^Ake^Ame^Ase;
        BCi = Abi^Agi^Aki^Ami^Asi;
        BCo = Abo^Ago^Ako^Amo^Aso;
        BCu = Abu^Agu^Aku^Amu^Asu;

        //thetaRhoPiChiIotaPrepareTheta(round, A, E)
        Da = BCu^ROL(BCe, 1);
        De = BCa^ROL(BCi, 1);
        Di = BCe^ROL(BCo, 1);
        Do = BCi^ROL(BCu, 1);
        Du = BCo^ROL(BCa, 1);

        myxorVer(Aba, Age, Aki, Amo, Asu, Da, De, Di, Do, Du);
        BCa = Aba;
        BCe = ROL(Age, 44);
        BCi = ROL(Aki, 43);
        BCo = ROL(Amo, 21);
        BCu = ROL(Asu, 14);
        computeVer(Eba, Ebe, Ebi, Ebo, Ebu, BCa, BCe, BCi, BCo, BCu);
        Eba ^= (uint64_t)KeccakF_RoundConstants[round];

        myxorVer(Abo, Agu, Aka, Ame, Asi, Do, Du, Da, De, Di);
        BCa = ROL(Abo, 28);
        BCe = ROL(Agu, 20);
        BCi = ROL(Aka,  3);
        BCo = ROL(Ame, 45);
        BCu = ROL(Asi, 61);
        computeVer(Ega, Ege, Egi, Ego, Egu, BCa, BCe, BCi, BCo, BCu);

        myxorVer(Abe, Agi, Ako, Amu, Asa, De, Di, Do, Du, Da);
        BCa = ROL(Abe,  1);
        BCe = ROL(Agi,  6);
        BCi = ROL(Ako, 25);
        BCo = ROL(Amu,  8);
        BCu = ROL(Asa, 18);
        computeVer(Eka, Eke, Eki, Eko, Eku, BCa, BCe, BCi, BCo, BCu);

        myxorVer(Abu, Aga, Ake, Ami, Aso, Du, Da, De, Di, Do);
        BCa = ROL(Abu, 27);
        BCe = ROL(Aga, 36);
        BCi = ROL(Ake, 10);
        BCo = ROL(Ami, 15);
        BCu = ROL(Aso, 56);
        computeVer(Ema, Eme, Emi, Emo, Emu, BCa, BCe, BCi, BCo, BCu);

        myxorVer(Abi, Ago, Aku, Ama, Ase, Di, Do, Du, Da, De);
        BCa = ROL(Abi, 62);
        BCe = ROL(Ago, 55);
        BCi = ROL(Aku, 39);
        BCo = ROL(Ama, 41);
        BCu = ROL(Ase,  2);
        computeVer(Esa, Ese, Esi, Eso, Esu, BCa, BCe, BCi, BCo, BCu);

        Aba = Eba;
        Abe = Ebe;
        Abi = Ebi;
        Abo = Ebo;
        Abu = Ebu;
        Aga = Ega;
        Age = Ege;
        Agi = Egi;
        Ago = Ego;
        Agu = Egu;
        Aka = Eka;
        Ake = Eke;
        Aki = Eki;
        Ako = Eko;
        Aku = Eku;
        Ama = Ema;
        Ame = Eme;
        Ami = Emi;
        Amo = Emo;
        Amu = Emu;
        Asa = Esa;
        Ase = Ese;
        Asi = Esi;
        Aso = Eso;
        Asu = Esu;
    }

    //copyToState(state, A)
    state[ 0] = Aba;
    state[ 1] = Abe;
    state[ 2] = Abi;
    state[ 3] = Abo;
    state[ 4] = Abu;
    state[ 5] = Aga;
    state[ 6] = Age;
    state[ 7] = Agi;
    state[ 8] = Ago;
    state[ 9] = Agu;
    state[10] = Aka;
    state[11] = Ake;
    state[12] = Aki;
    state[13] = Ako;
    state[14] = Aku;
    state[15] = Ama;
    state[16] = Ame;
    state[17] = Ami;
    state[18] = Amo;
    state[19] = Amu;
    state[20] = Asa;
    state[21] = Ase;
    state[22] = Asi;
    state[23] = Aso;
    state[24] = Asu;
}

template <typename T>
static void readmem(hls::stream<T> &out, T *in)
{
readmem:
    out << in[0];

}

template <typename T>
static void readmem_special(hls::stream<T> &out, T *in, unsigned int elements)
{
    for (unsigned int j = 0; j < elements; j++)
#pragma HLS PIPELINE II=1
        out << in[j];
}

template <typename T>
static void duplicate(hls::stream<T> &out1, hls::stream<T> &out2,
                  hls::stream<T> &in, int size)
{
duplicate:
T t = in.read();
out1 << t;
out2 << t;
for (unsigned int i = 1; i < size; i++) {
#pragma HLS PIPELINE II=1
    T t = in.read();
    out1 << t;
    out2 << t;
}
}

template <typename T>
static void triple(hls::stream<T> &out1, hls::stream<T> &out2,
               hls::stream<T> &out3, hls::stream<T> &in, int size)
{
duplicate:
for (unsigned int i = 0; i < size; i++) {
#pragma HLS PIPELINE II=1
    T t = in.read();
    out1 << t;
    out2 << t;
    out3 << t;
}
}

template <typename T>
static void quadruple(hls::stream<T> &out1, hls::stream<T> &out2,
                  hls::stream<T> &out3, hls::stream<T> &out4,
                  hls::stream<T> &in)
{
duplicate:
    T t = in.read();
    out1 << t;
    out2 << t;
    out3 << t;
    out4 << t;
}

template <typename T>
static void writemem(T *out, hls::stream<T> &stream, unsigned int size)
{
writemem:
for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II=1
    out[i] = stream.read();
}
}

template <typename T>
static void print(hls::stream<T> &out, hls::stream<T> &in,
              //unsigned int packn, unsigned int n, char *str)
              unsigned int packn, unsigned int n)
{
for (unsigned int p = 0; p < packn; p++)
    for (unsigned int i = 0; i < n; i++) {
        T t = in.read();
        //printf("fpga:%s -> [%d] = %d\n", str, i, t);
        printf("fpga[%d] = %d\n", i, t);
        out << t;
    }
}

static int32_t montgomery_reduce(int64_t a)
{
int32_t t;

t = (int64_t)(int32_t)a * QINV;
t = (a - (int64_t)t * Q) >> 32;
return t;
}

static void eta_unpack(hls::stream<int32_t> &out, hls::stream<uint8_t> &in,
                   unsigned int times)
{
#pragma HLS INLINE
for (unsigned int t = 0; t < times; t++) {
#if ETA == 2
    for (unsigned int i = 0; i < NI / 8; i++)  {
#pragma HLS PIPELINE II=8
        uint8_t a0 = in.read();
        uint8_t a1 = in.read();
        uint8_t a2 = in.read();

        int32_t r;
        r = (a0 >> 0) & 7;
        r = ETA - r;
        out << r;
        r =  (a0 >> 3) & 7;
        r = ETA - r;
        out << r;
        r = ((a0 >> 6) | (a1 << 2)) & 7;
        r = ETA - r;
        out << r;
        r =  (a1 >> 1) & 7;
        r = ETA - r;
        out << r;
        r =  (a1 >> 4) & 7;
        r = ETA - r;
        out << r;
        r = ((a1 >> 7) | (a2 << 1)) & 7;
        r = ETA - r;
        out << r;
        r =  (a2 >> 2) & 7;
        r = ETA - r;
        out << r;
        r =  (a2 >> 5) & 7;
        r = ETA - r;
        out << r;
    }
#elif ETA == 4
    for (unsigned int i = 0; i < NI / 2; i++)  {
#pragma HLS PIPELINE II=2
        uint8_t a = in.read();

        int32_t r;
        r = a & 0x0F;
        r = ETA - r;
        out << r;

        r = a >> 4;
        r = ETA - r;
        out << r;
    }
#endif
}
}

static void unpack_sk(hls::stream<uint8_t> &s_rho,
                  hls::stream<uint8_t> &s_key, hls::stream<int32_t> &s_t0,
                  hls::stream<int32_t> &s_s1, hls::stream<int32_t> &s_s2,
                  hls::stream<uint8_t> &s_sk)
{
    for (unsigned int i = 0; i < SEEDBYTES; i++) {
#pragma HLS PIPELINE II=1
        s_rho << s_sk.read();
    }
    for (unsigned int i = 0; i < SEEDBYTES; i++) {
#pragma HLS PIPELINE II=1
        s_key << s_sk.read();
    }
    for (unsigned int i = 0; i < TRBYTES; i++) {
#pragma HLS PIPELINE II=1
        uint8_t discard = s_sk.read();
    }
    eta_unpack(s_s1, s_sk, L);
    eta_unpack(s_s2, s_sk, K);
    for (unsigned int i = 0; i < K * NI / 8; i++) {
#pragma HLS PIPELINE II=13
        uint8_t a0  = s_sk.read();
        uint8_t a1  = s_sk.read();
        uint8_t a2  = s_sk.read();
        uint8_t a3  = s_sk.read();
        uint8_t a4  = s_sk.read();
        uint8_t a5  = s_sk.read();
        uint8_t a6  = s_sk.read();
        uint8_t a7  = s_sk.read();
        uint8_t a8  = s_sk.read();
        uint8_t a9  = s_sk.read();
        uint8_t a10 = s_sk.read();
        uint8_t a11 = s_sk.read();
        uint8_t a12 = s_sk.read();

        int32_t r;
        r  = a0;
        r |= (uint32_t)a1 << 8;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;

        r  = a1 >> 5;
        r |= (uint32_t)a2 << 3;
        r |= (uint32_t)a3 << 11;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;

        r  = a3 >> 2;
        r |= (uint32_t)a4 << 6;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;

        r  = a4 >> 7;
        r |= (uint32_t)a5 << 1;
        r |= (uint32_t)a6 << 9;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;

        r  = a6 >> 4;
        r |= (uint32_t)a7 << 4;
        r |= (uint32_t)a8 << 12;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;

        r  = a8 >> 1;
        r |= (uint32_t)a9 << 7;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;

        r  = a9 >> 6;
        r |= (uint32_t)a10 << 2;
        r |= (uint32_t)a11 << 10;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;

        r  = a11 >> 3;
        r |= (uint32_t)a12 << 5;
        r &= 0x1FFF;
        r = (1 << (D-1)) - r;
        s_t0 << r;
    }
}

static void unpack_sig(hls::stream<int> &s_ver, hls::stream<uint8_t> &s_c,
                   hls::stream<int32_t> &s_z, hls::stream<int32_t> &s_h,
                   hls::stream<uint8_t> &s_sig, unsigned int nbatch)
{
for (unsigned int b = 0; b < nbatch; b++) {
    for (unsigned int i = 0; i < CTILDEBYTES; i++) {
#pragma HLS PIPELINE II=1
        s_c << s_sig.read();
    }
#if GAMMA1 == (1 << 17)
    for(unsigned int i = 0; i < L * NI / 4; i++) {
        uint8_t a0 = s_sig.read();
        uint8_t a1 = s_sig.read();
        uint8_t a2 = s_sig.read();
        uint8_t a3 = s_sig.read();
        uint8_t a4 = s_sig.read();
        uint8_t a5 = s_sig.read();
        uint8_t a6 = s_sig.read();
        uint8_t a7 = s_sig.read();
        uint8_t a8 = s_sig.read();
        int32_t t;

        t  = a0;
        t |= (uint32_t)a1 << 8;
        t |= (uint32_t)a2 << 16;
        t &= 0x3FFFF;
        s_z << GAMMA1 - t;

        t  = a2 >> 2;
        t |= (uint32_t)a3 << 6;
        t |= (uint32_t)a4 << 14;
        t &= 0x3FFFF;
        s_z << GAMMA1 - t;

        t  = a4 >> 4;
        t |= (uint32_t)a5 << 4;
        t |= (uint32_t)a6 << 12;
        t &= 0x3FFFF;
        s_z << GAMMA1 - t;

        t  = a6 >> 6;
        t |= (uint32_t)a7 << 2;
        t |= (uint32_t)a8 << 10;
        t &= 0x3FFFF;
        s_z << GAMMA1 - t;
  }
#elif GAMMA1 == (1 << 19)
    for(unsigned int i = 0; i < L * NI / 2; i++) {
        uint8_t a0 = s_sig.read();
        uint8_t a1 = s_sig.read();
        uint8_t a2 = s_sig.read();
        uint8_t a3 = s_sig.read();
        uint8_t a4 = s_sig.read();
        int32_t t;

        t  = a0;
        t |= (uint32_t)a1 << 8;
        t |= (uint32_t)a2 << 16;
        t &= 0xFFFFF;
        s_z << GAMMA1 - t;

        t  = a2 >> 4;
        t |= (uint32_t)a3 << 4;
        t |= (uint32_t)a4 << 12;
        t &= 0xFFFFF;
        s_z << GAMMA1 - t;
    }
#endif
    int ret = 0;
    uint8_t sig[POLYVECH_PACKEDBYTES];
    for (unsigned int i = 0; i < POLYVECH_PACKEDBYTES; i++) {
        sig[i] = s_sig.read();
    }
    unsigned int k = 0;
    for (unsigned int i = 0; i < K; i++) {
        int32_t h[NI];
        for (unsigned int j = 0; j < NI; j++) {
            h[j] = 0;
        }
        if (sig[OMEGA + i] < k || sig[OMEGA + i] > OMEGA) {
            ret = -1;
        }
        for(unsigned int j = k; j < sig[OMEGA + i]; j++) {
            if (j > k && sig[j] <= sig[j-1]) {
                ret = -1;
            }
            h[sig[j]] = 1;
        }
        for (unsigned int j = 0; j < NI; j++) {
            s_h << h[j];
        }
        k = sig[OMEGA + i];
    }
    for (unsigned int i = k; i < OMEGA; i++) {
        if (sig[i]) {
            ret = -1;
        }
    }
    s_ver << ret;
}
}

static void chknorm(hls::stream<int> &s_ver, hls::stream<int32_t> &s_z, unsigned int times, int32_t bound)
{
int32_t B = bound;
    int ret = 0;
    if (B > (Q - 1) / 8)
        ret = -1;
    for (unsigned int i = 0; i < times * NI; i++) {
#pragma HLS PIPELINE II=1
        int32_t a = s_z.read();
        int32_t t = a >> 31;
        t = a - (t & 2 * a);
        if (t >= B)
            ret = -1;
    }
    s_ver << ret;
}

/*
* Begin of keccak
*/
static void keccak_init(uint64_t s[25])
{
for (unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
    s[i] = 0;
}
}

static void absorb_rate(uint64_t s[25], hls::stream<uint8_t> &in, unsigned int inlen)
{
#pragma HLS INLINE OFF
for (unsigned int j = 0; j < inlen; j++) {
#pragma HLS PIPELINE II=1
    s[j / 8] ^= (uint64_t)in.read() << 8 * (j % 8);
}
}

static void absorb_rate2(uint64_t s[25], hls::stream<ap_uint<16>> &in, unsigned int inlen)
{
#pragma HLS INLINE OFF
for (unsigned int j = 0; j < inlen; j += 2) {
#pragma HLS PIPELINE II=1
    ap_uint<16> t = in.read();
    s[j / 8] ^= (uint64_t)t.range(7, 0) << 8 * (j % 8);
    s[(j + 1) / 8] ^= (uint64_t)t.range(15, 8) << 8 * ((j + 1) % 8);
}
}

/*
* pos is always 0 upon initing the function the way I am
* because I am merging the input streams
*/
static void keccak_absorb(uint64_t s[25],
                      unsigned int &pos,
                      unsigned int r,
                      hls::stream<uint8_t> &in,
                      size_t inlen)
{
unsigned int blocks = inlen / r;
for (unsigned int i = 0; i < blocks; i++) {
#pragma HLS PIPELINE OFF
    absorb_rate(s, in, r);

    KeccakF1600_StatePermute(s);
}

inlen -= blocks * r;
absorb_rate(s, in, inlen);
pos = inlen;
}

static void keccak_absorb2(uint64_t s[25],
                      unsigned int &pos,
                      unsigned int r,
                      hls::stream<ap_uint<16>> &in,
                      size_t inlen)
{
unsigned int blocks = inlen / r;
for (unsigned int i = 0; i < blocks; i++) {
#pragma HLS PIPELINE OFF
    absorb_rate2(s, in, r);

    KeccakF1600_StatePermute(s);
}

inlen -= blocks * r;
absorb_rate2(s, in, inlen);
pos = inlen;
}

static void keccak_absorb_key_mu(uint64_t s[25],
                             hls::stream<uint8_t> &in1,
                             hls::stream<uint8_t> &in2)
{
for (unsigned int j = 0; j < SEEDBYTES + CRHBYTES + RNDBYTES; j++) {
#pragma HLS PIPELINE II=1
    uint8_t t;
    if (j < SEEDBYTES)
        t = in1.read();
    else if(j >= SEEDBYTES && j < SEEDBYTES + RNDBYTES)
    	t = 0;
	else
        t = in2.read();
    s[j / 8] ^= (uint64_t)t << 8 * (j % 8);
}
}

static void keccak_absorb_mu_p(uint64_t s[25],
                           hls::stream<uint8_t> &in1,
                           hls::stream<uint8_t> &in2)
{
//block 1
for (unsigned int j = 0; j < SHAKE256_RATE; j++) {
#pragma HLS PIPELINE II=1
    uint8_t t;
    if (j < CRHBYTES)
        t = in1.read();
    else
        t = in2.read();
    s[j / 8] ^= (uint64_t)t << 8 * (j % 8);
}

KeccakF1600_StatePermute(s);

unsigned int blocks = (CRHBYTES + K * POLYW1_PACKEDBYTES - SHAKE256_RATE) /
                      SHAKE256_RATE;
for (unsigned int i = 0; i < blocks; i++) {
#pragma HLS PIPELINE OFF
    absorb_rate(s, in2, SHAKE256_RATE);
    KeccakF1600_StatePermute(s);
}

unsigned int inlen = (CRHBYTES + K * POLYW1_PACKEDBYTES) % SHAKE256_RATE;
absorb_rate(s, in2, inlen);
}

static void keccak_finalize(uint64_t s[25], unsigned int pos, unsigned int r, bool ver)
{
uint8_t p = 0x1F;
s[pos / 8] ^= (uint64_t)p << 8 * (pos % 8);
if (ver)
    s[(r - 1) / 8] ^= 1ULL << 63;
else
    s[r / 8 - 1] ^= 1ULL << 63;
}

/*
* We are sure that inlen < r -> no permutes needed
*/
static void make_buf_state(uint64_t s[25], hls::stream<uint8_t> &in, bool doit)
{
#pragma HLS INLINE OFF
if (doit) {
    keccak_init(s);
    absorb_rate(s, in, SEEDBYTES + 2);
    keccak_finalize(s, SEEDBYTES + 2, SHAKE128_RATE, 0);
}
}

static void squeeze_out(hls::stream<uint8_t> &out, unsigned int outlen,
                    uint64_t s[25], unsigned int &pos)
{
#pragma HLS INLINE OFF
for (unsigned int i = 0; i < outlen; i++) {
#pragma HLS PIPELINE II=1
    uint8_t t = s[pos / 8] >> 8 * (pos % 8);
    out << t;
    pos++;
}
}

static void squeeze_out4(hls::stream<ap_uint<32>> &out, unsigned int outlen,
                    uint64_t s[25], unsigned int &pos)
{
#pragma HLS INLINE OFF
for (unsigned int i = 0; i < outlen; i += 4) {
#pragma HLS PIPELINE II=1
    ap_uint<32> r;
    uint8_t t = s[pos / 8] >> 8 * (pos % 8);
    r(7, 0) = t;
    t = s[(pos + 1) / 8] >> 8 * ((pos + 1) % 8);
    r(15, 8) = t;
    t = s[(pos + 2) / 8] >> 8 * ((pos + 2) % 8);
    r(23, 16) = t;
    t = s[(pos + 3) / 8] >> 8 * ((pos + 3) % 8);
    r(31, 24) = t;
    out << r;
    pos += 4;
}
}

/*
* outlen is SHAKE128_RATE which is a multiple of 3
*/
static void make_buf_squeeze_out(hls::stream<ap_uint<24>> &out, uint64_t s[25])
{
#pragma HLS INLINE OFF
for (unsigned int i = 0; i < SHAKE128_RATE; i += 3) {
#pragma HLS PIPELINE II=1
    ap_uint<24> r;
    uint8_t t = s[i / 8] >> 8 * (i % 8);
    r(7, 0) = t;
    t = s[(i + 1) / 8] >> 8 * ((i + 1) % 8);
    r(15, 8) = t;
    t = s[(i + 2) / 8] >> 8 * ((i + 2) % 8);
    r(23, 16) = t;
    out << r;
}
}

static void copy_state(uint64_t dst[25], uint64_t src[25])
{
for (unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
    dst[i] = src[i];
}
}

/*
* All calls to squeeze happen after setting pos to r,
* so we immediately do the permutation
* This can be seen both in absorb_once and finalize functions
*/
static void keccak_squeeze(hls::stream<uint8_t> &out,
                               size_t outlen,
                               uint64_t s[25],
                               unsigned int &pos,
                               unsigned int r)
{
uint64_t s2[25];
#pragma HLS ARRAY_PARTITION variable=s2 complete

KeccakF1600_StatePermute(s);
copy_state(s2, s);
pos = 0;

unsigned int blocks = outlen / r;
for (unsigned int i = 0; i < blocks; i++) {
#pragma HLS PIPELINE OFF
    squeeze_out(out, r, s2, pos);

    KeccakF1600_StatePermute(s);
    copy_state(s2, s);
    pos = 0;
}
outlen -= blocks * r;
squeeze_out(out, outlen, s2, pos);
}

static void keccak_squeeze4(hls::stream<ap_uint<32>> &out,
                               size_t outlen,
                               uint64_t s[25],
                               unsigned int &pos,
                               unsigned int r)
{
uint64_t s2[25];
#pragma HLS ARRAY_PARTITION variable=s2 complete

KeccakF1600_StatePermute(s);
copy_state(s2, s);
pos = 0;

unsigned int blocks = outlen / r;
for (unsigned int i = 0; i < blocks; i++) {
#pragma HLS PIPELINE OFF
    squeeze_out4(out, r, s2, pos);

    KeccakF1600_StatePermute(s);
    copy_state(s2, s);
    pos = 0;
}
outlen -= blocks * r;
squeeze_out4(out, outlen, s2, pos);
}

/*
* outlen = POLY_UNIFORM_NBLOCKS * STREAM128_BLOCKBYTES
* we know that POLY_UNIFORM_NBLOCKS > 2 so we can use that minus 2 without issues
*/
static void make_buf_squeeze(hls::stream<ap_uint<24>> &out, hls::stream<uint8_t> &in,
                         uint64_t s[25], uint64_t s2[25], bool doit)
{
uint64_t s3[25];
#pragma HLS ARRAY_PARTITION variable=s3 complete

/*
 * case(i)
 *     POLY_UNIFORM_NBLOCKS - 2: absorb next
 *     POLY_UNIFORM_NBLOCKS - 1: permute next
 */
copy_state(s3, s);
for (unsigned int i = 0; i < POLY_UNIFORM_NBLOCKS - 2; i++) {
#pragma HLS PIPELINE OFF
    make_buf_squeeze_out(out, s3);
    KeccakF1600_StatePermute(s);
    copy_state(s3, s);
}
{
    make_buf_state(s2, in, doit);
    make_buf_squeeze_out(out, s3);
    KeccakF1600_StatePermute(s);
    copy_state(s3, s);
}
{
    make_buf_squeeze_out(out, s3);
    KeccakF1600_StatePermute(s2);
}

}

static void shake4(hls::stream<ap_uint<32>> &out, unsigned int outlen,
              hls::stream<ap_uint<16>> &in, unsigned int inlen,
              unsigned int r, bool version, unsigned int nbatch)
{
for (unsigned int b = 0; b < nbatch; b++) {
#pragma HLS PIPELINE OFF
    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
    unsigned int pos = 0;
    keccak_init(s);
    keccak_absorb2(s, pos, r, in, inlen);
    keccak_finalize(s, pos, r, version);
    keccak_squeeze4(out, outlen, s, pos, r);
}
}

static void shake_key_mu(hls::stream<uint8_t> &out,
                     hls::stream<uint8_t> &in1, hls::stream<uint8_t> &in2)
{

    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
    unsigned int pos = 0;
    keccak_init(s);
    keccak_absorb_key_mu(s, in1, in2);
    pos = SEEDBYTES + CRHBYTES + RNDBYTES;
    keccak_finalize(s, pos, SHAKE256_RATE, 1);
    keccak_squeeze(out, CRHBYTES, s, pos, SHAKE256_RATE);
}

static void shake_mu_p(hls::stream<uint8_t> &out,
                   hls::stream<uint8_t> &in1, hls::stream<uint8_t> &in2)
{
    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
    unsigned int pos = 0;
    keccak_init(s);
    keccak_absorb_mu_p(s, in1, in2);
    pos = (CRHBYTES + K * POLYW1_PACKEDBYTES) % SHAKE256_RATE;
    keccak_finalize(s, pos, SHAKE256_RATE, 0);
    keccak_squeeze(out, CTILDEBYTES, s, pos, SHAKE256_RATE);
}

//This can be optimized to save SEEDBYTS cycles
static void make_seed(hls::stream<uint8_t> &out, hls::stream<uint8_t> &in)
{
    uint8_t rho[SEEDBYTES];
    for (unsigned int k = 0; k < SEEDBYTES; k++) {
        rho[k] = in.read();
    }
    for (unsigned int i = 0; i < K; i++) {
        for (unsigned int j = 0; j < L; j++) {
            uint16_t nonce = (i << 8) + j;
            for (unsigned int k = 0; k < SEEDBYTES; k++) {
                out << rho[k];
            }
            uint8_t t = nonce;
            out << t;
            t = nonce >> 8;
            out << t;
        }
    }
}

static void make_buf(hls::stream<ap_uint<24>> &out, hls::stream<uint8_t> &in)
{
uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
uint64_t s2[25];
#pragma HLS ARRAY_PARTITION variable=s2 complete
make_buf_state(s2, in, true); //err
KeccakF1600_StatePermute(s2);
for (unsigned int i = 0; i < K * L; i++) {
#pragma HLS PIPELINE OFF
    copy_state(s, s2);
    make_buf_squeeze(out, in, s, s2, i != K * L - 1);
}
}

static void zunpack(hls::stream<int32_t> &out, hls::stream<ap_uint<32>> &in) {
#if GAMMA1 == (1 << 17)
for(unsigned int i = 0; i < L * NI / 4; i += 4) {
#pragma HLS PIPELINE II=16
	ap_uint<32> t0 = in.read();
    ap_uint<32> t1 = in.read();
    ap_uint<32> t2 = in.read();
    ap_uint<32> t3 = in.read();
    ap_uint<32> t4 = in.read();
    ap_uint<32> t5 = in.read();
    ap_uint<32> t6 = in.read();
    ap_uint<32> t7 = in.read();
    ap_uint<32> t8 = in.read();
    uint8_t a0  = t0.range(7 ,  0);
    uint8_t a1  = t0.range(15,  8);
    uint8_t a2  = t0.range(23, 16);
    uint8_t a3  = t0.range(31, 24);
    uint8_t a4  = t1.range(7 ,  0);
    uint8_t a5  = t1.range(15,  8);
    uint8_t a6  = t1.range(23, 16);
    uint8_t a7  = t1.range(31, 24);
    uint8_t a8  = t2.range(7 ,  0);
    uint8_t a9  = t2.range(15,  8);
    uint8_t a10 = t2.range(23, 16);
    uint8_t a11 = t2.range(31, 24);
    uint8_t a12 = t3.range(7 ,  0);
    uint8_t a13 = t3.range(15,  8);
    uint8_t a14 = t3.range(23, 16);
    uint8_t a15 = t3.range(31, 24);
    uint8_t a16 = t4.range(7 ,  0);
    uint8_t a17 = t4.range(15,  8);
    uint8_t a18 = t4.range(23, 16);
    uint8_t a19 = t4.range(31, 24);
    uint8_t a20 = t5.range(7 ,  0);
    uint8_t a21 = t5.range(15,  8);
    uint8_t a22 = t5.range(23, 16);
    uint8_t a23 = t5.range(31, 24);
    uint8_t a24 = t6.range(7 ,  0);
    uint8_t a25 = t6.range(15,  8);
    uint8_t a26 = t6.range(23, 16);
    uint8_t a27 = t6.range(31, 24);
    uint8_t a28 = t7.range(7 ,  0);
    uint8_t a29 = t7.range(15,  8);
    uint8_t a30 = t7.range(23, 16);
    uint8_t a31 = t7.range(31, 24);
    uint8_t a32 = t8.range(7 ,  0);
    uint8_t a33 = t8.range(15,  8);
    uint8_t a34 = t8.range(23, 16);
    uint8_t a35 = t8.range(31, 24);

    int32_t r;
    r  = a0;
    r |= (uint32_t)a1 << 8;
    r |= (uint32_t)a2 << 16;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a2 >> 2;
    r |= (uint32_t)a3 << 6;
    r |= (uint32_t)a4 << 14;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a4 >> 4;
    r |= (uint32_t)a5 << 4;
    r |= (uint32_t)a6 << 12;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a6 >> 6;
    r |= (uint32_t)a7 << 2;
    r |= (uint32_t)a8 << 10;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a9;
    r |= (uint32_t)a10 << 8;
    r |= (uint32_t)a11 << 16;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a11 >> 2;
    r |= (uint32_t)a12 << 6;
    r |= (uint32_t)a13 << 14;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a13 >> 4;
    r |= (uint32_t)a14 << 4;
    r |= (uint32_t)a15 << 12;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a15 >> 6;
    r |= (uint32_t)a16 << 2;
    r |= (uint32_t)a17 << 10;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a18;
    r |= (uint32_t)a19 << 8;
    r |= (uint32_t)a20 << 16;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a20 >> 2;
    r |= (uint32_t)a21 << 6;
    r |= (uint32_t)a22 << 14;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a22 >> 4;
    r |= (uint32_t)a23 << 4;
    r |= (uint32_t)a24 << 12;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a24 >> 6;
    r |= (uint32_t)a25 << 2;
    r |= (uint32_t)a26 << 10;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a27;
    r |= (uint32_t)a28 << 8;
    r |= (uint32_t)a29 << 16;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a29 >> 2;
    r |= (uint32_t)a30 << 6;
    r |= (uint32_t)a31 << 14;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a31 >> 4;
    r |= (uint32_t)a32 << 4;
    r |= (uint32_t)a33 << 12;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a33 >> 6;
    r |= (uint32_t)a34 << 2;
    r |= (uint32_t)a35 << 10;
    r &= 0x3FFFF;
    r = GAMMA1 - r;
    out << r;
}
#elif GAMMA1 == (1 << 19)
for(unsigned int i = 0; i < L * NI / 2; i += 4) {
#pragma HLS PIPELINE II=8

    ap_uint<32> t0 = in.read();
    ap_uint<32> t1 = in.read();
    ap_uint<32> t2 = in.read();
    ap_uint<32> t3 = in.read();
    ap_uint<32> t4 = in.read();
    uint8_t a0  = t0.range(7 ,  0);
    uint8_t a1  = t0.range(15,  8);
    uint8_t a2  = t0.range(23, 16);
    uint8_t a3  = t0.range(31, 24);
    uint8_t a4  = t1.range(7 ,  0);
    uint8_t a5  = t1.range(15,  8);
    uint8_t a6  = t1.range(23, 16);
    uint8_t a7  = t1.range(31, 24);
    uint8_t a8  = t2.range(7 ,  0);
    uint8_t a9  = t2.range(15,  8);
    uint8_t a10 = t2.range(23, 16);
    uint8_t a11 = t2.range(31, 24);
    uint8_t a12 = t3.range(7 ,  0);
    uint8_t a13 = t3.range(15,  8);
    uint8_t a14 = t3.range(23, 16);
    uint8_t a15 = t3.range(31, 24);
    uint8_t a16 = t4.range(7 ,  0);
    uint8_t a17 = t4.range(15,  8);
    uint8_t a18 = t4.range(23, 16);
    uint8_t a19 = t4.range(31, 24);

    int32_t r;
    r  = a0;
    r |= (uint32_t)a1 << 8;
    r |= (uint32_t)a2 << 16;
    r &= 0xFFFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a2 >> 4;
    r |= (uint32_t)a3 << 4;
    r |= (uint32_t)a4 << 12;
    r &= 0xFFFFF; // on original code this is wrongly written
    r = GAMMA1 - r;
    out << r;

    r  = a5;
    r |= (uint32_t)a6 << 8;
    r |= (uint32_t)a7 << 16;
    r &= 0xFFFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a7 >> 4;
    r |= (uint32_t)a8 << 4;
    r |= (uint32_t)a9 << 12;
    r &= 0xFFFFF; // on original code this is wrongly written
    r = GAMMA1 - r;
    out << r;

    r  = a10;
    r |= (uint32_t)a11 << 8;
    r |= (uint32_t)a12 << 16;
    r &= 0xFFFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a12 >> 4;
    r |= (uint32_t)a13 << 4;
    r |= (uint32_t)a14 << 12;
    r &= 0xFFFFF; // on original code this is wrongly written
    r = GAMMA1 - r;
    out << r;

    r  = a15;
    r |= (uint32_t)a16 << 8;
    r |= (uint32_t)a17 << 16;
    r &= 0xFFFFF;
    r = GAMMA1 - r;
    out << r;

    r  = a17 >> 4;
    r |= (uint32_t)a18 << 4;
    r |= (uint32_t)a19 << 12;
    r &= 0xFFFFF; // on original code this is wrongly written
    r = GAMMA1 - r;
    out << r;
}
#endif
}

static void make_seed_gamma(hls::stream<ap_uint<16>> &out, hls::stream<uint8_t> &in,
                        uint16_t nonce)
{
unsigned int lnonce = nonce;
uint8_t seed[CRHBYTES];
for (unsigned int i = 0; i < CRHBYTES; i++) {
    seed[i] = in.read();
}

for (unsigned int i = 0; i < L; i++) {
        ap_uint<16> r;
        for (unsigned int j = 0; j < CRHBYTES; j += 2) {
            r(7, 0) = seed[j];
            r(15,8) = seed[j + 1];
            out << r;

        }
        uint16_t non = L * lnonce + i;
        r(7,  0) = non;
        r(15, 8) = (non >> 8);
        out << r;

}
}

template <unsigned int X, typename T>
static void repeatg(hls::stream<T> &out, hls::stream<T> &in)
{
    T tmp[X];
    for (unsigned int i = 0; i < X; i++) {
#pragma HLS PIPELINE II=1
        T t = in.read();
        tmp[i] = t;
        out << t;
    }
}

template <typename T>
static void repeatkl(hls::stream<T> &out, hls::stream<T> &in)
{
    T tmp[NI * K * L];
    for (unsigned int i = 0; i < K * L * NI; i++) {
#pragma HLS PIPELINE II=1
        T t = in.read();
        tmp[i] = t;
        out << t;
    }
}

template <typename T>
static void repeatc(hls::stream<T> &out, hls::stream<T> &in)
{
    T tmp[CRHBYTES];
    for (unsigned int i = 0; i < CRHBYTES; i++) {
#pragma HLS PIPELINE II=1

        T t = in.read();
        tmp[i] = t;
        out << t;
    }
}

template <typename T>
static void repeatk(hls::stream<T> &out, hls::stream<T> &in)
{
    T tmp[NI * K];
    for (unsigned int i = 0; i < K * NI; i++) {
#pragma HLS PIPELINE II=1
        T t = in.read();
        tmp[i] = t;
        out << t;
    }
}

static void add(hls::stream<int32_t> &out, hls::stream<int32_t> &in1,
            hls::stream<int32_t> &in2, unsigned int times)
{
for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1

    out << in1.read() + in2.read();
}
}

static void pdecompose(hls::stream<int32_t> &out1, hls::stream<int32_t> &out2,
                   hls::stream<int32_t> &in, unsigned int times)
{
for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
    int32_t r1, r2;
    int32_t t = in.read();
    r1 = decomposeVer(&r2, t);
    out1 << r1;
    out2 << r2;
}
}

static void make_hint(hls::stream<int> &signal, hls::stream<int32_t> &out,
                  hls::stream<int32_t> &in1, hls::stream<int32_t> &in2,
                  unsigned int times)
{
unsigned int s = 0;
for (unsigned int i = 0; i < times; i++) {
#pragma HLS PIPELINE II=1
    int32_t a0 = in1.read();
    int32_t a1 = in2.read();
    unsigned int t;
    if(a0 > GAMMA2 || a0 < -GAMMA2 || (a0 == -GAMMA2 && a1 != 0))
        t = 1;
    else
        t = 0;
    s += t;
    out << t;
}
signal << s;
}

static void pack_sig(hls::stream<uint8_t> &out, hls::stream<uint8_t> &in1,
                 hls::stream<int32_t> &in2, hls::stream<int32_t> &in3)
{
for (unsigned int i = 0; i < CTILDEBYTES; i++) {
#pragma HLS PIPELINE II=1
    out << in1.read();
}
#if GAMMA1 == (1 << 17)
for(unsigned int i = 0; i < L * NI / 4; i++) {
#pragma HLS PIPELINE II=9
    int32_t a0 = in2.read();
    int32_t a1 = in2.read();
    int32_t a2 = in2.read();
    int32_t a3 = in2.read();

    uint32_t t0 = GAMMA1 - a0;
    uint32_t t1 = GAMMA1 - a1;
    uint32_t t2 = GAMMA1 - a2;
    uint32_t t3 = GAMMA1 - a3;

    uint8_t r;
    r  = t0;
    out << r;
    r  = t0 >> 8;
    out << r;
    r  = t0 >> 16;
    r |= t1 << 2;
    out << r;
    r  = t1 >> 6;
    out << r;
    r  = t1 >> 14;
    r |= t2 << 4;
    out << r;
    r  = t2 >> 4;
    out << r;
    r  = t2 >> 12;
    r |= t3 << 6;
    out << r;
    r  = t3 >> 2;
    out << r;
    r  = t3 >> 10;
    out << r;
}
#elif GAMMA1 == (1 << 19)
for(unsigned int i = 0; i < L * NI / 2; i++) {
#pragma HLS PIPELINE II=5
    int32_t a0 = in2.read();
    int32_t a1 = in2.read();

    uint32_t t0 = GAMMA1 - a0;
    uint32_t t1 = GAMMA1 - a1;

    uint8_t r;
    r  = t0;
    out << r;
    r  = t0 >> 8;
    out << r;
    r  = t0 >> 16;
    r |= t1 << 4;
    out << r;
    r  = t1 >> 4;
    out << r;
    r  = t1 >> 12;
    out << r;
}
#endif

uint8_t sig[OMEGA + K];
uint8_t last[K];
#pragma HLS ARRAY_PARTITION variable=last complete

unsigned int k = 0;
unsigned int wr = 0;
for (unsigned int i = 0; i < K; i++) {
    for (unsigned int j = 0; j < NI; j++) {
#pragma HLS PIPELINE II=1
        if (in3.read() != 0) {
            if (k < OMEGA) {
                out << j;
                wr++;
            }
            else if (k < OMEGA + K && k - OMEGA != i)
                last[k - OMEGA] = j;
            k++;
        }
    }
    last[i] = k;
}

for (unsigned int i = wr; i < OMEGA + K; i++) {
#pragma HLS PIPELINE II=1
    if (i < OMEGA)
        out << 0;
    else
        out << last[i - OMEGA];
}
}

static void write_discard(uint8_t *out1, uint8_t *out2,
                      hls::stream<int> &s_signal_z,
                      hls::stream<int> &s_signal_w0,
                      hls::stream<int> &s_signal_h,
                      hls::stream<int> &s_signal_n,
                      hls::stream<ap_uint<1>> &s_signal_ch,
                      //hls::stream<int> &s_signal_rej,
					  hls::stream<ap_uint<1>> &s_signal_rej,
                      hls::stream<uint8_t> &in,
                      unsigned int &complete)
{
complete = 0;
bool done = false;
unsigned int discard = 0;
unsigned int ret = 0;
if (s_signal_z.read() == -1)
    discard = 1;
if (s_signal_w0.read() == -1)
    discard = 1;
if (s_signal_h.read() == -1)
    discard = 1;
if (s_signal_n.read() > OMEGA)
    discard = 1;
if (s_signal_ch.read())
    ret = 1;
for (unsigned int i = 0; i < K * L; i++) {
#pragma HLS PIPELINE II=1
    if (s_signal_rej.read())
        ret = 1;
}

if (!done && !discard) {
    for (unsigned int i = 0; i < CRYPTO_BYTES; i++) {
#pragma HLS PIPELINE II=1
        out2[i] = in.read();
    }
    out1[0] = ret;
    done = true;
    complete++;
}
else {
    for (unsigned int i = 0; i < CRYPTO_BYTES; i++) {
#pragma HLS PIPELINE II=1
        uint8_t t = in.read();
    }
}
}

static void dataflow(uint8_t *ret, uint8_t *sig,
                 uint8_t *mu_0, uint8_t *mu_1, uint8_t *sk,
                 unsigned int n, uint16_t nonce, unsigned int &done)
{
#pragma HLS INLINE OFF
/*
 * Stream declaration begin
 */
static hls::stream<uint8_t> s_sk("s_sk");
#pragma HLS STREAM variable=s_sk depth=100*2
static hls::stream<uint8_t> s_rho("s_rho");
#pragma HLS STREAM variable=s_rho depth=seedbytes+2*2
static hls::stream<uint8_t> s_key("s_key");
#pragma HLS STREAM variable=s_key depth=seedbytes+2*2
static hls::stream<int32_t> s_t0("s_t0");
#pragma HLS STREAM variable=s_t0 depth=paramk*paramn*2
static hls::stream<int32_t> s_s1("s_s1");
#pragma HLS STREAM variable=s_s1 depth=paramk*paramn*2
static hls::stream<int32_t> s_s2("s_s2");
#pragma HLS STREAM variable=s_s2 depth=paramk*paramn*2
static hls::stream<uint8_t> s_mu("s_mu");
#pragma HLS STREAM variable=s_mu depth=crhbytes+2*2
static hls::stream<uint8_t> s_rhoprime("s_rhoprime");
#pragma HLS STREAM variable=s_rhoprime depth=crhbytes+2*2
static hls::stream<uint8_t> s_seed("s_seed");
#pragma HLS STREAM variable=s_seed depth=3000
static hls::stream<ap_uint<24>> s_mat_buf("s_mat_buf");
#pragma HLS STREAM variable=s_mat_buf depth=500*2
static hls::stream<int32_t> s_mat("s_mat");
#pragma HLS STREAM variable=s_mat depth=paramk*paraml*paramn*2
static hls::stream<int32_t> s_mat_r("s_mat_r");
#pragma HLS STREAM variable=s_mat_r depth=paramk*paraml*paramn*2
static hls::stream<int32_t> s_t0_t("s_t0_t");
#pragma HLS STREAM variable=s_t0_t depth=paramk*paramn*2
static hls::stream<int32_t> s_s1_t("s_s1_t");
#pragma HLS STREAM variable=s_s1_t depth=paraml*paramn*2
static hls::stream<int32_t> s_s2_t("s_s2_t");
#pragma HLS STREAM variable=s_s2_t depth=paramk*paramn*2
static hls::stream<int32_t> s_t0_t_r("s_t0_t_r");
#pragma HLS STREAM variable=s_t0_t_r depth=paramk*paramn*2
static hls::stream<int32_t> s_s1_t_r("s_s1_t_r");
#pragma HLS STREAM variable=s_s1_t_r depth=paramk*paramn*2
static hls::stream<int32_t> s_s2_t_r("s_s2_t_r");
#pragma HLS STREAM variable=s_s2_t_r depth=paramk*paramn*2
static hls::stream<uint8_t> s_mu_0("s_mu_0");
#pragma HLS STREAM variable=s_mu_0 depth=500*2
static hls::stream<uint8_t> s_mu_1("s_mu_1");
#pragma HLS STREAM variable=s_mu_1 depth=500*2
static hls::stream<uint8_t> s_mu_1_r("s_mu_1_r");
#pragma HLS STREAM variable=s_mu_1_r depth=500*2
static hls::stream<uint8_t> s_key_mu("s_key_mu");
#pragma HLS STREAM variable=s_key_mu depth=500*2
static hls::stream<ap_uint<16>> s_seed_gamma("s_seed_gamma");
#pragma HLS STREAM variable=s_seed_gamma depth=500*2
static hls::stream<ap_uint<32>> s_buf_gamma("s_buf_gamma");
#pragma HLS STREAM variable=s_buf_gamma depth=3*paramn*2
static hls::stream<int32_t> s_y("s_y");
#pragma HLS STREAM variable=s_y depth=paraml*paramn*2
static hls::stream<int32_t> s_y_1("s_y_1");
#pragma HLS STREAM variable=s_y_1 depth=paraml*paramn*2
static hls::stream<int32_t> s_z("s_z");
#pragma HLS STREAM variable=s_z depth=paraml*paramn*2
static hls::stream<int32_t> s_z_t("s_z_t");
#pragma HLS STREAM variable=s_z_t depth=paraml*paramn*2
static hls::stream<int32_t> s_z_rl("s_z_rl");
#pragma HLS STREAM variable=s_z_rl depth=paraml*paramn*2
static hls::stream<int32_t> s_w1_m("s_w1_m");
#pragma HLS STREAM variable=s_w1_m depth=paramk*paramn*2
static hls::stream<int32_t> s_w1("s_w1");
#pragma HLS STREAM variable=s_w1 depth=paramk*paramn*2
static hls::stream<int32_t> s_w1_r("s_w1_r");
#pragma HLS STREAM variable=s_w1_r depth=paramk*paramn*2
static hls::stream<int32_t> s_w1_i("s_w1_i");
#pragma HLS STREAM variable=s_w1_i depth=paramk*paramn*2
static hls::stream<int32_t> s_w1_f("s_w1_f");
#pragma HLS STREAM variable=s_w1_f depth=paramk*paramn*2
static hls::stream<int32_t> s_w1_c("s_w1_c");
#pragma HLS STREAM variable=s_w1_c depth=paramk*paramn*2
static hls::stream<int32_t> s_w1_d("s_w1_d");
#pragma HLS STREAM variable=s_w1_d depth=paramk*paramn*2
static hls::stream<int32_t> s_w0("s_w0");
#pragma HLS STREAM variable=s_w0 depth=paramk*paramn*2
static hls::stream<int32_t> s_w1_d_0("s_w1_d_0");
#pragma HLS STREAM variable=s_w1_d_0 depth=paramk*paramn*2
static hls::stream<int32_t> s_w1_d_1("s_w1_d_0");
#pragma HLS STREAM variable=s_w1_d_1 depth=paramk*paramn*2
static hls::stream<uint8_t> s_p("s_p");
#pragma HLS STREAM variable=s_p depth=500*2
static hls::stream<uint8_t> s_mu_p("s_mu_p");
#pragma HLS STREAM variable=s_mu_p depth=500*2
static hls::stream<uint8_t> s_sig("s_sig");
#pragma HLS STREAM variable=s_sig depth=500*2
static hls::stream<uint8_t> s_sig_0("s_sig_0");
#pragma HLS STREAM variable=s_sig_0 depth=500*2
static hls::stream<uint8_t> s_sig_1("s_sig_1");
#pragma HLS STREAM variable=s_sig_1 depth=500*2
static hls::stream<int32_t> s_cp("s_cp");
#pragma HLS STREAM variable=s_cp depth=500*2
static hls::stream<int32_t> s_cp_t("s_cp_t");
#pragma HLS STREAM variable=s_cp_t depth=paramn*2
static hls::stream<int32_t> s_cp_t_0("s_cp_t_0");
#pragma HLS STREAM variable=s_cp_t_0 depth=paramn*2
static hls::stream<int32_t> s_cp_t_0_r("s_cp_t_0_r");
#pragma HLS STREAM variable=s_cp_t_0_r depth=paraml*paramn*2
static hls::stream<int32_t> s_cp_t_1("s_cp_t_1");
#pragma HLS STREAM variable=s_cp_t_1 depth=paramn*2
static hls::stream<int32_t> s_cp_t_1_r("s_cp_t_1_r");
#pragma HLS STREAM variable=s_cp_t_1_r depth=paramk*paramn*2
static hls::stream<int32_t> s_cp_t_2("s_cp_t_2");
#pragma HLS STREAM variable=s_cp_t_2 depth=paramn*2
static hls::stream<int32_t> s_cp_t_2_r("s_cp_t_2_r");
#pragma HLS STREAM variable=s_cp_t_2_r depth=paramk*paramn*2
static hls::stream<int32_t> s_z_m("s_z_m");
#pragma HLS STREAM variable=s_z_m depth=paraml*paramn*2
static hls::stream<int32_t> s_z_i("s_z_i");
#pragma HLS STREAM variable=s_z_i depth=paraml*paramn*2
static hls::stream<int32_t> s_z_f("s_z_f");
#pragma HLS STREAM variable=s_z_f depth=paraml*paramn*2
static hls::stream<int32_t> s_z_a("s_z_a");
#pragma HLS STREAM variable=s_z_a depth=paraml*paramn*2
static hls::stream<int32_t> s_z_r("s_z_r");
#pragma HLS STREAM variable=s_z_r depth=paraml*paramn*2
static hls::stream<int32_t> s_z_r_0("s_z_r_0");
#pragma HLS STREAM variable=s_z_r_0 depth=paraml*paramn*2
static hls::stream<int32_t> s_z_r_1("s_z_r_1");
#pragma HLS STREAM variable=s_z_r_1 depth=paraml*paramn*2
static hls::stream<int> s_signal_z("s_signal_z");
#pragma HLS STREAM variable=s_signal_z depth=100*2
static hls::stream<int32_t> s_h_m("s_h_m");
#pragma HLS STREAM variable=s_h_m depth=paramk*paramn*2
static hls::stream<int32_t> s_h_i("s_h_i");
#pragma HLS STREAM variable=s_h_i depth=paramk*paramn*2
static hls::stream<int32_t> s_h_f("s_h_f");
#pragma HLS STREAM variable=s_h_f depth=paramk*paramn*2
static hls::stream<int32_t> s_w0_s("s_w0_s");
#pragma HLS STREAM variable=s_w0_s depth=paramk*paramn*2
static hls::stream<int32_t> s_w0_r("s_w0_r");
#pragma HLS STREAM variable=s_w0_r depth=paramk*paramn*2
static hls::stream<int32_t> s_w0_r_0("s_w0_r_0");
#pragma HLS STREAM variable=s_w0_r_0 depth=paramk*paramn*2
static hls::stream<int32_t> s_w0_r_1("s_w0_r_1");
#pragma HLS STREAM variable=s_w0_r_1 depth=paramk*paramn*2
static hls::stream<int> s_signal_w0("s_signal_w0");
#pragma HLS STREAM variable=s_signal_w0 depth=100*2
static hls::stream<int32_t> s_h_m_2("s_h_m_2");
#pragma HLS STREAM variable=s_h_m_2 depth=paramk*paramn*2
static hls::stream<int32_t> s_h_i_2("s_h_i_2");
#pragma HLS STREAM variable=s_h_i_2 depth=paramk*paramn*2
static hls::stream<int32_t> s_h_f_2("s_h_f_2");
#pragma HLS STREAM variable=s_h_f_2 depth=paramk*paramn*2
static hls::stream<int32_t> s_h_r("s_h_r");
#pragma HLS STREAM variable=s_h_r depth=paramk*paramn*2
static hls::stream<int32_t> s_h_r_0("s_h_r_0");
#pragma HLS STREAM variable=s_h_r_0 depth=paramk*paramn*2
static hls::stream<int32_t> s_h_r_1("s_h_r_1");
#pragma HLS STREAM variable=s_h_r_1 depth=paramk*paramn*2
static hls::stream<int> s_signal_h("s_signal_h");
#pragma HLS STREAM variable=s_signal_h depth=100*2
static hls::stream<int32_t> s_w0_a("s_w0_a");
#pragma HLS STREAM variable=s_w0_a depth=paramk*paramn*2
static hls::stream<int> s_signal_n("s_signal_n");
#pragma HLS STREAM variable=s_signal_n depth=100*2
static hls::stream<int32_t> s_h_h("s_h_h");
#pragma HLS STREAM variable=s_h_h depth=paramk*paramn*2
static hls::stream<uint8_t> s_sig_p("s_sig_p");
#pragma HLS STREAM variable=s_sig_p depth=crypto*2
static hls::stream<ap_uint<1>> s_signal_ch("s_signal_ch");
#pragma HLS STREAM variable=s_signal_ch depth=100*2
static hls::stream<ap_uint<1>> s_signal_rej("s_signal_rej");
#pragma HLS STREAM variable=s_signal_rej depth=100*2


/*
 * Stream declaration end
 */
#pragma HLS DATAFLOW


readmemVer(s_sk, sk, CRYPTO_SECRETKEYBYTES);
unpack_sk(s_rho, s_key, s_t0, s_s1, s_s2, s_sk);


readmemVer(s_mu_0, mu_0, CRHBYTES);
shake_key_mu(s_rhoprime, s_key, s_mu_0);

make_seed(s_seed, s_rho); // no share and see the config
make_buf(s_mat_buf, s_seed); // no share
rej_uniformVer(s_mat, s_mat_buf, s_signal_rej);
repeatkl(s_mat_r, s_mat); //check if its possible mix all

nttVer(s_s1_t, s_s1, L);
repeatg<NI * L>(s_s1_t_r, s_s1_t);
nttVer(s_s2_t, s_s2, K);
repeatg<NI * K>(s_s2_t_r, s_s2_t);
nttVer(s_t0_t, s_t0, K);
repeatg<NI * K>(s_t0_t_r, s_t0_t);

rej:
//uniform_gamma1
make_seed_gamma(s_seed_gamma, s_rhoprime, nonce); // no share and see the config
#if GAMMA1 == (1 << 17)
shake4(s_buf_gamma, 9 * NI / 4,
#elif GAMMA1 == (1 << 19)
shake4(s_buf_gamma, 5 * NI / 2,
#endif
      s_seed_gamma, CRHBYTES + 2,
      SHAKE256_RATE, 0, L);
zunpack(s_y, s_buf_gamma);

duplicateVer(s_z, s_y_1, s_y, L * NI);

nttVer(s_z_t, s_z, L);
repeatVerlVer(s_z_rl, s_z_t, K);
montgomeryVer(s_w1_m, s_mat_r, s_z_rl, K * L * NI);
montgomeryVer_add(s_w1, s_w1_m, K);

reduce32Ver(s_w1_r, s_w1, K * NI);
invnttVer(s_w1_i, s_w1_r, K);
reducefVer(s_w1_f, s_w1_i, K * NI);

caddqVer(s_w1_c, s_w1_f, K * NI);
pdecompose(s_w1_d, s_w0, s_w1_c, K * NI);
duplicateVer(s_w1_d_0, s_w1_d_1, s_w1_d, K * NI);
packVer(s_p, s_w1_d_0, K);

readmemVer(s_mu_1, mu_1, CRHBYTES);
repeatg<CRHBYTES>(s_mu_1_r, s_mu_1);
shake_mu_p(s_sig, s_mu_1_r, s_p); //check how to share all shakes

duplicateVer(s_sig_0, s_sig_1, s_sig, SEEDBYTES);

challengeVer(s_cp, s_sig_0, s_signal_ch);
nttVer(s_cp_t, s_cp, 1);
triple(s_cp_t_0, s_cp_t_1, s_cp_t_2, s_cp_t, NI);

compute_z:
repeatVer(s_cp_t_0_r, s_cp_t_0, L);
montgomeryVer(s_z_m, s_cp_t_0_r, s_s1_t_r, L * NI);
invnttVer(s_z_i, s_z_m, L);
reducefVer(s_z_f, s_z_i, L * NI);
add(s_z_a, s_z_f, s_y_1, L * NI);
reduce32Ver(s_z_r, s_z_a, L * NI);
duplicateVer(s_z_r_0, s_z_r_1, s_z_r, L * NI);
chknormVer(s_signal_z, s_z_r_0, L, GAMMA1 - BETA);

compute_w0:
repeatVer(s_cp_t_1_r, s_cp_t_1, K);
montgomeryVer(s_h_m, s_cp_t_1_r, s_s2_t_r, K * NI);
invnttVer(s_h_i, s_h_m, K);
reducefVer(s_h_f, s_h_i, K * NI);
subVer(s_w0_s, s_w0, s_h_f, K * NI);
reduce32Ver(s_w0_r, s_w0_s, K * NI);
duplicateVer(s_w0_r_0, s_w0_r_1, s_w0_r, K * NI);
chknormVer(s_signal_w0, s_w0_r_0, K, GAMMA2 - BETA);

compute_h:
repeatVer(s_cp_t_2_r, s_cp_t_2, K);
montgomeryVer(s_h_m_2, s_cp_t_2_r, s_t0_t_r, K * NI);
invnttVer(s_h_i_2, s_h_m_2, K);
reducefVer(s_h_f_2, s_h_i_2, K * NI);
reduce32Ver(s_h_r, s_h_f_2, K * NI);
duplicateVer(s_h_r_0, s_h_r_1, s_h_r, K * NI);
chknormVer(s_signal_h, s_h_r_0, K, GAMMA2);

add(s_w0_a, s_w0_r_1, s_h_r_1, K * NI);
make_hint(s_signal_n, s_h_h, s_w0_a, s_w1_d_1, K * NI);

pack_sig(s_sig_p, s_sig_1, s_z_r_1, s_h_h);
write_discard(ret, sig,
              s_signal_z, s_signal_w0,
              s_signal_h, s_signal_n, s_signal_ch, s_signal_rej,
              s_sig_p, done);
}

extern "C" {
void k_sign(uint8_t *ret, uint8_t *sig, uint8_t *mu, uint8_t *mu2,
        uint8_t *sk)
{
unsigned int n = 0;
uint16_t nonce = 0;
while (n < 1) {
#pragma HLS PIPELINE OFF
    unsigned int done;
    dataflow(ret, sig, mu, mu2, sk, n, nonce, done);
    nonce += 1;
    n += done;
}
}
}



void mldsa_accelerator(unsigned char kem_cfg, 
                    uint8_t *ret_out,
                    uint8_t *sign_out,
                    uint8_t *sign_in,
                    uint8_t *mu_processed_in,
                    uint8_t *mu_orig_in,                    
                    uint8_t *mu2_processed_in,
                    uint8_t *sk_in,
                    uint8_t *pk_in,
                    int *ver_out,
                    size_t mlen_in)
                    

{

    //VERIFICATION
    #pragma HLS INTERFACE m_axi port=ver_out   		depth=64       	offset=slave bundle=gmemout
    #pragma HLS INTERFACE m_axi port=sign_in   		depth=2620    	offset=slave bundle=gmemsign
    #pragma HLS INTERFACE m_axi port=mu_orig_in 	depth=2600    	offset=slave bundle=gmemm
    #pragma HLS INTERFACE m_axi port=pk_in          depth=2600 		offset=slave bundle=gmempk


    //Signature
    #pragma HLS INTERFACE m_axi port=ret_out         	depth=64    offset=slave bundle=gmemout
    #pragma HLS INTERFACE m_axi port=sign_out           depth=2620	offset=slave bundle=gmemsign
    #pragma HLS INTERFACE m_axi port=mu_processed_in    depth=64	offset=slave bundle=gmemm
    #pragma HLS INTERFACE m_axi port=mu2_processed_in   depth=64	offset=slave bundle=gmempk
    #pragma HLS INTERFACE m_axi port=sk_in              depth=2628	offset=slave bundle=gmemsk
 






    #pragma HLS INTERFACE s_axilite port=ret_out            bundle=control
    #pragma HLS INTERFACE s_axilite port=sign_out           bundle=control
    #pragma HLS INTERFACE s_axilite port=mu_processed_in    bundle=control
    #pragma HLS INTERFACE s_axilite port=mu2_processed_in   bundle=control
    #pragma HLS INTERFACE s_axilite port=sk_in              bundle=control


    #pragma HLS INTERFACE s_axilite port=ver_out            bundle=control
    #pragma HLS INTERFACE s_axilite port=sign_in            bundle=control
    #pragma HLS INTERFACE s_axilite port=mu_orig_in         bundle=control
    #pragma HLS INTERFACE s_axilite port=pk_in              bundle=control
    
    #pragma HLS INTERFACE s_axilite port=mlen_in            bundle=control

    #pragma HLS INTERFACE s_axilite port=kem_cfg            bundle=control
    #pragma HLS INTERFACE s_axilite port=return             bundle=control

    if(kem_cfg == 0){
        k_sign(ret_out, sign_out, mu_processed_in, mu2_processed_in, sk_in);
    } else {
        k_verify(ver_out, sign_in, mu_orig_in, mlen_in, pk_in);
    }
}
























