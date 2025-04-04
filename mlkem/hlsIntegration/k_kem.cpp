// Copyright (c) 2025 Barcelona Supercomputing Center
// Licensed under the Solderpad Hardware License, Version 0.51 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//     http://solderpad.org/licenses/SHL-0.51/
//
// Unless required by applicable law or agreed to in writing, software,
// hardware and materials distributed under this License are distributed
// on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language
// governing permissions and limitations under the License.
//
// SPDX-License-Identifier: SHL-0.51

#include "kernel.hpp"

const unsigned int bytes = KYBER_INDCPA_BYTES;
const unsigned int skbytes = KYBER_INDCPA_SECRETKEYBYTES;
const unsigned int pkbytes = KYBER_INDCPA_PUBLICKEYBYTES;
const unsigned int rate = SHAKE256_RATE;
const unsigned int symbytes = KYBER_SYMBYTES;
const unsigned int kyber = KYBER_K;
const unsigned int kybern = KYBER_N;
const unsigned int eta1 = KYBER_ETA1;
const unsigned int eta2 = KYBER_ETA2;

const int16_t zetas[128] = {
  -1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
   -171,   622,  1577,   182,   962, -1202, -1474,  1468,
    573, -1325,   264,   383,  -829,  1458, -1602,  -130,
   -681,  1017,   732,   608, -1542,   411,  -205, -1571,
   1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
    516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
   -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
   -398,   961, -1508,  -725,   448, -1065,   677, -1275,
  -1103,   430,   555,   843, -1251,   871,  1550,   105,
    422,   587,   177,  -235,  -291,  -460,  1574,  1653,
   -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
  -1590,   644,  -872,   349,   418,   329,  -156,   -75,
    817,  1097,   603,   610,  1322, -1285, -1465,   384,
  -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
  -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
   -108,  -308,   996,   991,   958, -1460,  1522,  1628
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

static void compute(uint64_t &A, uint64_t &B, uint64_t &C, uint64_t &D, uint64_t &E,
                    uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
#pragma HLS INLINE OFF
    A = a ^ ((~b) & c);
    B = b ^ ((~c) & d);
    C = c ^ ((~d) & e);
    D = d ^ ((~e) & a);
    E = e ^ ((~a) & b);
}

static void myxor(uint64_t &A, uint64_t &B, uint64_t &C, uint64_t &D, uint64_t &E,
                  uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e)
{
#pragma HLS INLINE OFF
    A ^= a;
    B ^= b;
    C ^= c;
    D ^= d;
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

            myxor(Aba, Age, Aki, Amo, Asu, Da, De, Di, Do, Du);
            BCa = Aba;
            BCe = ROL(Age, 44);
            BCi = ROL(Aki, 43);
            BCo = ROL(Amo, 21);
            BCu = ROL(Asu, 14);
            compute(Eba, Ebe, Ebi, Ebo, Ebu, BCa, BCe, BCi, BCo, BCu);
            Eba ^= (uint64_t)KeccakF_RoundConstants[round];

            myxor(Abo, Agu, Aka, Ame, Asi, Do, Du, Da, De, Di);
            BCa = ROL(Abo, 28);
            BCe = ROL(Agu, 20);
            BCi = ROL(Aka,  3);
            BCo = ROL(Ame, 45);
            BCu = ROL(Asi, 61);
            compute(Ega, Ege, Egi, Ego, Egu, BCa, BCe, BCi, BCo, BCu);

            myxor(Abe, Agi, Ako, Amu, Asa, De, Di, Do, Du, Da);
            BCa = ROL(Abe,  1);
            BCe = ROL(Agi,  6);
            BCi = ROL(Ako, 25);
            BCo = ROL(Amu,  8);
            BCu = ROL(Asa, 18);
            compute(Eka, Eke, Eki, Eko, Eku, BCa, BCe, BCi, BCo, BCu);

            myxor(Abu, Aga, Ake, Ami, Aso, Du, Da, De, Di, Do);
            BCa = ROL(Abu, 27);
            BCe = ROL(Aga, 36);
            BCi = ROL(Ake, 10);
            BCo = ROL(Ami, 15);
            BCu = ROL(Aso, 56);
            compute(Ema, Eme, Emi, Emo, Emu, BCa, BCe, BCi, BCo, BCu);

            myxor(Abi, Ago, Aku, Ama, Ase, Di, Do, Du, Da, De);
            BCa = ROL(Abi, 62);
            BCe = ROL(Ago, 55);
            BCi = ROL(Aku, 39);
            BCo = ROL(Ama, 41);
            BCu = ROL(Ase,  2);
            compute(Esa, Ese, Esi, Eso, Esu, BCa, BCe, BCi, BCo, BCu);

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
static void readmem(hls::stream<T> &out, T *in, int size)
{
readmem:
    for (int i = 0; i < size; i++) {
        out << in[i];
    }
}

template <typename T>
static void duplicate(hls::stream<T> &out1, hls::stream<T> &out2,
                      hls::stream<T> &in, int size)
{
duplicate:
    for (unsigned int i = 0; i < size; i++) {
#pragma HLS PIPELINE II=1
        T t = in.read();
        out1 << t;
        out2 << t;
    }
}

static void sk_split(hls::stream<uint8_t> &out1, hls::stream<uint8_t> &out2,
                     hls::stream<uint8_t> &out3, hls::stream<uint8_t> &out4, 
                     hls::stream<uint8_t> &in, int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
        for (unsigned int i = 0; i < KYBER_INDCPA_SECRETKEYBYTES; i++) {
#pragma HLS PIPELINE II=1
            out1 << in.read();
        }
        for (unsigned int i = 0; i < KYBER_INDCPA_PUBLICKEYBYTES; i++) {
#pragma HLS PIPELINE II=1
            out2 << in.read();
        }
        for (unsigned int i = 0; i < KYBER_SYMBYTES; i++) {
#pragma HLS PIPELINE II=1
            out3 << in.read();
        }
        for (unsigned int i = 0; i < KYBER_SYMBYTES; i++) {
#pragma HLS PIPELINE II=1
            out4 << in.read();
        }
    }
}

template <typename T>
static void triple(hls::stream<T> &out1, hls::stream<T> &out2, hls::stream<T> &out3,
                   hls::stream<T> &in, int size)
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

static void decompress(hls::stream<int16_t> &out1, hls::stream<int16_t> &out2,
                       hls::stream<uint8_t> &in, unsigned int times)
{
    int16_t tmp;
    for (unsigned int p = 0; p < times; p++) {
        /*
         * polyvec
         */
#if (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 352))
        uint16_t t[8];
        for (unsigned int i = 0; i < KYBER_K; i++) {
            for (unsigned int j = 0; j < KYBER_N / 8; j++) {
#pragma HLS PIPELINE II=8
                uint8_t a0 = in.read();
                uint8_t a1 = in.read();
                uint8_t a2 = in.read();
                uint8_t a3 = in.read();
                uint8_t a4 = in.read();
                uint8_t a5 = in.read();
                uint8_t a6 = in.read();
                uint8_t a7 = in.read();
                uint8_t a8 = in.read();
                uint8_t a9 = in.read();
                uint8_t a10 = in.read();
                t[0] = (a0 >> 0) | ((uint16_t)a1 << 8);
                t[1] = (a1 >> 3) | ((uint16_t)a2 << 5);
                t[2] = (a2 >> 6) | ((uint16_t)a3 << 2) | ((uint16_t)a4 << 10);
                t[3] = (a4 >> 1) | ((uint16_t)a5 << 7);
                t[4] = (a5 >> 4) | ((uint16_t)a6 << 4);
                t[5] = (a6 >> 7) | ((uint16_t)a7 << 1) | ((uint16_t)a8 << 9);
                t[6] = (a8 >> 2) | ((uint16_t)a9 << 6);
                t[7] = (a9 >> 5) | ((uint16_t)a10 << 3);

                for (unsigned int k = 0; k < 8; k++) {
                    tmp = ((uint32_t)(t[k] & 0x7FF) * KYBER_Q + 1024) >> 11;
                    out1 << tmp;
                }    
            }
        }
#elif (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 320))
        uint16_t t[4];
        for (unsigned int i = 0; i < KYBER_K; i++) {
            for (unsigned int j = 0; j < KYBER_N / 4; j++) {
#pragma HLS PIPELINE II=4
                uint8_t a0 = in.read();
                uint8_t a1 = in.read();
                uint8_t a2 = in.read();
                uint8_t a3 = in.read();
                uint8_t a4 = in.read();
                t[0] = (a0 >> 0) | ((uint16_t)a1 << 8);
                t[1] = (a1 >> 2) | ((uint16_t)a2 << 6);
                t[2] = (a2 >> 4) | ((uint16_t)a3 << 4);
                t[3] = (a3 >> 6) | ((uint16_t)a4 << 2);
                for (unsigned int k = 0; k < 4; k++) {
                    tmp = ((uint32_t)(t[k] & 0x3FF) * KYBER_Q + 512) >> 10;
                    out1 << tmp;
                }
            }
        }

#else
#error "KYBER_POLYVECCOMPRESSEDBYTES needs to be in {320*KYBER_K, 352*KYBER_K}"
#endif

        /*
        * poly
        */
#if (KYBER_POLYCOMPRESSEDBYTES == 128)
        for (unsigned int i = 0; i < KYBER_N / 2; i++) {
#pragma HLS PIPELINE II=2
            uint8_t t = in.read();
            tmp = (((uint16_t)(t & 15) * KYBER_Q) + 8) >> 4;
            out2 << tmp;
            tmp = (((uint16_t)(t >> 4) * KYBER_Q) + 8) >> 4;
            out2 << tmp;
        }
#elif (KYBER_POLYCOMPRESSEDBYTES == 160)
        uint8_t t2[8];
        for(unsigned int i = 0; i < KYBER_N / 8; i++) {
#pragma HLS PIPELINE II=8
            uint8_t a0 = in.read();
            uint8_t a1 = in.read();
            uint8_t a2 = in.read();
            uint8_t a3 = in.read();
            uint8_t a4 = in.read();
            t2[0] = (a0 >> 0);
            t2[1] = (a0 >> 5) | (a1 << 3);
            t2[2] = (a1 >> 2);
            t2[3] = (a1 >> 7) | (a2 << 1);
            t2[4] = (a2 >> 4) | (a3 << 4);
            t2[5] = (a3 >> 1);
            t2[6] = (a3 >> 6) | (a4 << 2);
            t2[7] = (a4 >> 3);

            for(unsigned int j = 0; j < 8; j++) {
                tmp = ((uint32_t)(t2[j] & 31) * KYBER_Q + 16) >> 5;
                out2 << tmp;
            }
        }
#else
#error "KYBER_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
    }
}

static void frombytes(hls::stream<int16_t> &out, hls::stream<uint8_t> &in,
                      unsigned int packn)
{
    for (unsigned int p = 0; p < packn * KYBER_N / 2 * KYBER_K; p++) {
#pragma HLS PIPELINE II=3
        int16_t tmp;
        uint8_t a0 = in.read();
        uint8_t a1 = in.read();
        uint8_t a2 = in.read();
        tmp = ((a0 >> 0) | ((uint16_t)a1 << 8)) & 0xFFF;
        out << tmp;
        tmp = ((a1 >> 4) | ((uint16_t)a2 << 4)) & 0xFFF;
        out << tmp;
    }
}

static void sub_reduce(hls::stream<int16_t> &out, hls::stream<int16_t> &in1,
                      hls::stream<int16_t> &in2, unsigned int packn)
{
    for (unsigned int p = 0; p < packn * KYBER_N; p++) {
        out << in1.read() - in2.read();
    }
}

static void tomsg(hls::stream<uint8_t> &out, hls::stream<int16_t> &in,
                  unsigned int packn)
{
    for (unsigned int p = 0; p < packn * KYBER_N / 8; p++) {
#pragma HLS PIPELINE II=8
        uint8_t v = 0;
        for (unsigned int i = 0; i < 8; i++) {
            int16_t t = in.read();
            t += ((int16_t)t >> 15) & KYBER_Q;
            t  = (((t << 1) + KYBER_Q/2)/KYBER_Q) & 1;
            v |= t << i;
        }
        out << v;
    }
}

static void tmp_read(uint64_t s[25], hls::stream<uint8_t> &in, unsigned int r)
{
#pragma HLS INLINE OFF
    for(unsigned int i = 0; i < r / 8; i++) {
#pragma HLS PIPELINE II=8
        uint64_t r = 0;
        for (unsigned int j = 0; j < 8; j++) {
            r |= (uint64_t)in.read() << 8 * j;
        }
        s[i] ^= r;
    }
}

static void sha1_write(hls::stream<uint8_t> &out1,
                       hls::stream<uint8_t> &out2,
                       uint64_t s[25])
{
#pragma HLS INLINE OFF
    unsigned int outlen = KYBER_SYMBYTES;
    for (unsigned int i = 0; i < outlen / 8; i++) {
#pragma HLS PIPELINE II=8
        for (unsigned int j = 0; j < 8; j++) {
            uint8_t t = (uint8_t)(s[i] >> 8 * j);
            out1 << t;
            out2 << t;
        }
    }
}

static void sha1_read(uint64_t s[25], hls::stream<uint8_t> &in)
{
#pragma HLS INLINE OFF
    unsigned int i;
    unsigned int r = SHA3_256_RATE;
    uint8_t p = 0x06;
    for(i = 0; i < KYBER_SYMBYTES; i++) {
#pragma HLS PIPELINE II=1
        s[i / 8] ^= (uint64_t)in.read() << 8 * (i % 8);
    }
    s[i / 8] ^= (uint64_t)p << 8*(i%8);
    s[(r - 1) / 8] ^= 1ULL << 63;
}

static void kyber_sha(hls::stream<uint8_t> &out,
                      hls::stream<uint8_t> &in1, hls::stream<uint8_t> &in2,
                      unsigned int packn)
{
sha:
    unsigned int r = SHA3_256_RATE;
    uint8_t p = 0x06;
    unsigned int inlen;
    unsigned int outlen = KYBER_SYMBYTES;
    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete

    for (unsigned int pn = 0; pn < packn; pn++) {
#pragma HLS PIPELINE OFF
sha1:
        /*inlen = KYBER_SYMBYTES;
        for (unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s[i] = 0;
            s2[i] = 0;
        }*/
        /* KYBER_SYMBYTES < SHA3_256_RATE -> ignore
        for (unsigned int o = 0; o < inlen / r; o++) {
            tmp_read(s, in1, r);
            KeccakF1600_StatePermute(s);
        }
        */

        /*{
        sha1_read(s, in1);
        KeccakF1600_StatePermute(s);
        sha1_write(out1, out2, s);
        tmp_read(s2, in2, r);
        }*/
        for (unsigned int i = 0; i < outlen; i++) {
            out << in1.read();
        }
        inlen = KYBER_PUBLICKEYBYTES;
        for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s[i] = 0;
        }

        tmp_read(s, in2, r);

        KeccakF1600_StatePermute(s);

        for (unsigned int o = 1; o < inlen / r; o++) {
#pragma HLS PIPELINE OFF
            tmp_read(s, in2, r);
            KeccakF1600_StatePermute(s);
        }
        unsigned int i;
        for(i = 0; i < inlen - (inlen / r) * r; i++) {
            s[i / 8] ^= (uint64_t)in2.read() << 8 * (i % 8);
        }

        s[i / 8] ^= (uint64_t)p << 8*(i%8);
        s[(r - 1) / 8] ^= 1ULL << 63;
        KeccakF1600_StatePermute(s);
        for (unsigned int i = 0; i < outlen / 8; i++) {
            for (unsigned int j = 0; j < 8; j++) {
                out << (uint8_t)(s[i] >> 8 * j);
            }
        }
    }
}

static void sha(hls::stream<uint8_t> &out, unsigned int outlen,
                hls::stream<uint8_t> &in, unsigned int inlen,
                unsigned int r, unsigned int times)
{
sha:
    uint8_t p = 0x06;
    for (unsigned int tm = 0; tm < times; tm++) {
#pragma HLS PIPELINE OFF
        uint64_t s[25];

#pragma HLS ARRAY_PARTITION variable=s complete
        for (unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s[i] = 0;
        }

        for (unsigned int o = 0; o < inlen / r; o++) {
#pragma HLS PIPELINE OFF
            tmp_read(s, in, r);
            KeccakF1600_StatePermute(s);
        }

        unsigned int i;
        for(i = 0; i < inlen - (inlen / r) * r; i++) {
            s[i / 8] ^= (uint64_t)in.read() << 8 * (i % 8);
        }

        s[i / 8] ^= (uint64_t)p << 8*(i%8);
        s[(r - 1) / 8] ^= 1ULL << 63;
        KeccakF1600_StatePermute(s);
        for (unsigned int i = 0; i < outlen / 8; i++) {
            for (unsigned int j = 0; j < 8; j++) {
                out << (uint8_t)(s[i] >> 8 * j);
            }
        }
    }
}

static void make_coins(hls::stream<ap_uint<256>> &out, hls::stream<uint8_t> &in,
                    unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
#pragma HLS PIPELINE II=32
        ap_uint<256> t;
        for (unsigned int i = 0; i < 32; i++) {
            t((i + 1) * 8 - 1, i * 8) = in.read();
        }
        out <<t ;
    }

}
/*
 * stream &in: clear
 */
static void get_key(hls::stream<ap_uint<264>> &out, hls::stream<ap_uint<256>> &in,
                    unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
#pragma HLS PIPELINE II=1
        uint8_t nonce = 0;
        ap_uint<264> var;
        
        var(255, 0) = in.read();
        for (unsigned int k = 0; k < 2 * KYBER_K + 1; k++) {
            var(263, 256) = nonce++;
            out << var;
        }
    }
}

static void absorb(uint64_t s[25], hls::stream<ap_uint<264>> &in, bool act)
{
#pragma HLS INLINE OFF
    if (act) {
        uint8_t p = 0x1F;
        unsigned int i;
#pragma HLS INLINE OFF
        for(i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s[i] = 0;
        }
        
        ap_uint<264> var = in.read();
        for(i = 0; i < KYBER_SYMBYTES + 1; i++) {
#pragma HLS UNROLL
            s[i / 8] ^= (uint64_t)var.range((i + 1) * 8 - 1, i * 8) << 8 * (i % 8);
        }

        s[i / 8] ^= (uint64_t)p << 8 * (i % 8);
        s[(SHAKE256_RATE - 1) / 8] ^= 1ULL << 63;
    }
}

static void squeeze(hls::stream<uint8_t> &out1, hls::stream<uint8_t> &out2,
                    uint64_t s[25], unsigned int limit, bool first)
{
#pragma HLS INLINE OFF
    for (unsigned int i = 0; i < limit; i++) {
#pragma HLS PIPELINE II=1
        uint8_t t = (uint8_t)(s[i / 8] >> 8 * (i % 8));
        if (first)
            out1 << t;
        else
            out2 << t;
    }
}

static void absorb_permute(uint64_t s[25], hls::stream<ap_uint<264>> &in, bool act)
{
#pragma HLS INLINE OFF
    absorb(s, in, act);
    KeccakF1600_StatePermute(s);
}

static void kyber_shake(hls::stream<uint8_t> &out1, unsigned int outlen1,
                        hls::stream<uint8_t> &out2, unsigned int outlen2,
                        hls::stream<ap_uint<264>> &in, unsigned int packn)
{
shake:
    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
    uint64_t s2[25];
#pragma HLS ARRAY_PARTITION variable=s2 complete
    absorb_permute(s, in, true);

    for (unsigned int tm = 0; tm < packn * (2 * KYBER_K + 1); tm++) {
#pragma HLS PIPELINE OFF
        unsigned int outlen;
        unsigned int first = (tm % (2 * KYBER_K + 1)) < KYBER_K;
        if (first) {
            outlen = outlen1;
        }
        else {
            outlen = outlen2;
        }

        // squeezeblocks begin
        for (unsigned int n = 0; n < outlen / SHAKE256_RATE; n++) {
            for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
                s2[i] = s[i];
            }
            squeeze(out1, out2, s2, SHAKE256_RATE, first);
            absorb_permute(s, in, false);
        }
        // squeezeblocks end

        // squeeze begin
        unsigned int limit = outlen - outlen / SHAKE256_RATE * SHAKE256_RATE;

squeeze:
        for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s2[i] = s[i];
        }
        squeeze(out1, out2, s2, limit, first);
        absorb_permute(s, in, tm < packn * (2 * KYBER_K + 1) - 1);
        // squeeze end
    }
}

static void copy_state(uint64_t dst[25], uint64_t src[25])
{
    for (unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
        dst[i] = src[i];
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

static void keccak_init(uint64_t s[25])
{
    for (unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
        s[i] = 0;
    }
}
static void keccak_absorb(uint64_t s[25],
                          unsigned int &pos,
                          unsigned int r,
                          hls::stream<uint8_t> &in,
                          size_t inlen)
{
 unsigned int i;

  while(pos+inlen >= r) {
    #pragma HLS PIPELINE OFF
    for(i=pos;i<r;i++) 
      s[i/8] ^= (uint64_t)in.read() << 8*(i%8);
    inlen -= r-pos;
    KeccakF1600_StatePermute(s);
    pos = 0;
  }

  for(i=pos;i<pos+inlen;i++)
    s[i/8] ^= (uint64_t)in.read() << 8*(i%8);

  pos = i;
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

static void rkprf(hls::stream<uint8_t> &out, hls::stream<uint8_t> &key, hls::stream<uint8_t> &in,
                    unsigned int packn)
{
    uint64_t s[25];
    #pragma HLS ARRAY_PARTITION variable=s complete
    for (unsigned int p = 0; p < packn; p++) {
        unsigned int pos = 0;
        keccak_init(s);
        keccak_absorb(s, pos, SHAKE256_RATE, key, KYBER_SYMBYTES);
        keccak_absorb(s, pos, SHAKE256_RATE, in, KYBER_CIPHERTEXTBYTES);
        keccak_finalize(s, pos, SHAKE256_RATE, 0);
        keccak_squeeze(out, KYBER_SSBYTES, s, pos, SHAKE256_RATE);
    }
}

static void make_kr(hls::stream<ap_uint<512>> &out, hls::stream<uint8_t> &in,
                    unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
#pragma HLS PIPELINE II=2*symbytes
        ap_uint<512> t;
        for (unsigned int i = 0; i < 2 * KYBER_SYMBYTES; i++) {
            t((i + 1) * 8 - 1, i * 8) = in.read();
        }
        out << t;
    }
}

static void absorb2(uint64_t s[25], hls::stream<ap_uint<512>> &in, bool act)
{
#pragma HLS INLINE OFF
    if (act) {
        uint8_t p = 0x1F;
        unsigned int i;
#pragma HLS INLINE OFF
        for(i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s[i] = 0;
        }
        
        ap_uint<512> var = in.read();
        for(i = 0; i < 2 * KYBER_SYMBYTES; i++) {
#pragma HLS UNROLL
            s[i / 8] ^= (uint64_t)var.range((i + 1) * 8 - 1, i * 8) << 8 * (i % 8);
        }

        s[i / 8] ^= (uint64_t)p << 8 * (i % 8);
        s[(SHAKE256_RATE - 1) / 8] ^= 1ULL << 63;
    }
}

static void squeeze2(hls::stream<uint8_t> &out, uint64_t s[25], unsigned int limit)
{
//#pragma HLS INLINE OFF
    for (unsigned int i = 0; i < limit; i++) {
#pragma HLS PIPELINE II=1
        uint8_t t = (uint8_t)(s[i / 8] >> 8 * (i % 8));
        out << t;
    }
}

static void absorb_permute2(uint64_t s[25], hls::stream<ap_uint<512>> &in, bool act)
{
#pragma HLS INLINE OFF
    absorb2(s, in, act);
    KeccakF1600_StatePermute(s);
}

static void shake256(hls::stream<uint8_t> &out, unsigned int outlen,
                     hls::stream<ap_uint<512>> &in, unsigned int times)
{
shake2:
    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
    uint64_t s2[25];
#pragma HLS ARRAY_PARTITION variable=s2 complete
    absorb_permute2(s, in, true);

    for (unsigned int tm = 0; tm < times; tm++) {
#pragma HLS PIPELINE OFF
        // squeezeblocks begin
        for (unsigned int n = 0; n < outlen / SHAKE256_RATE; n++) {
            for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
                s2[i] = s[i];
            }
            squeeze2(out, s2, SHAKE256_RATE);
            absorb_permute2(s, in, false);
        }
        // squeezeblocks end

        // squeeze begin
        unsigned int limit = outlen - outlen / SHAKE256_RATE * SHAKE256_RATE;

squeeze2:
        for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s2[i] = s[i];
        }
        squeeze2(out, s, limit);
        absorb_permute2(s, in, tm < times - 1);
        // squeeze end
    }
}

#if KYBER_ETA1 == 3
static void cbd3(hls::stream<int16_t> &out, hls::stream<uint8_t> &in, unsigned int times)
{
    for (unsigned int i = 0; i < times * KYBER_N / 4; i++) {
        uint8_t buf;
        uint32_t t;
        //load24_littleendian
        buf = in.read();
        t = (uint32_t)buf;
        buf = in.read();
        t |= (uint32_t)buf << 8;
        buf = in.read();
        t |= (uint32_t)buf << 16;
        uint32_t d  = t & 0x00249249;
        d += (t >> 1) & 0x00249249;
        d += (t >> 2) & 0x00249249;

        for (unsigned int j = 0; j < 4; j++) {
            int16_t a = (d >> (6 * j + 0)) & 0x7;
            int16_t b = (d >> (6 * j + 3)) & 0x7;
            out << a - b;
        }
    }
}
#endif

static void cbd2(hls::stream<int16_t> &out, hls::stream<uint8_t> &in, unsigned int times)
{
    for (unsigned int i = 0; i < times * KYBER_N / 8; i++) {
        uint8_t buf;
        uint32_t t;
        //load32_littleendian
        buf = in.read();
        t = (uint32_t)buf;
        buf = in.read();
        t |= (uint32_t)buf << 8;
        buf = in.read();
        t |= (uint32_t)buf << 16;
        buf = in.read();
        t |= (uint32_t)buf << 24;
        uint32_t d  = t & 0x55555555;
        d += (t>>1) & 0x55555555;

        for (unsigned int j = 0; j < 8; j++) {
            int16_t a = (d >> (4 * j + 0)) & 0x3;
            int16_t b = (d >> (4 * j + 2)) & 0x3;
            out << a - b;
        }
    }
}

static void read_mem(hls::stream<int16_t> &out, int16_t *in, int size)
{
read_mem:
    for (int i = 0; i < size; i++) {
        out << in[i];
    }
}

template <typename T>
static void split(hls::stream<T> &out1, hls::stream<T> &out2,
                  hls::stream<T> &in, int size, int split)
{
    int counter = 0;
stream_split:
    for (int i = 0; i < size; i++) {
        T t = in.read();
        if (counter < split) {
            out1 << t;
            counter++;
        }
        else {
            out2 << t;
            counter++;
            if (counter == (split << 1)) {
                counter = 0;
            }
        }
    }
}

static void ntt_butterfly(int16_t *x, int16_t *y, int16_t z)
{
    int32_t pr = (int32_t)z * (*y);
    int16_t tmp = (int16_t) pr * QINV;
    tmp = (pr - (int32_t)tmp * KYBER_Q) >> 16;
    (*y) = (*x) - tmp;
    (*x) = (*x) + tmp;
}


static void ntt_layer(hls::stream<int16_t> &sink_0, hls::stream<int16_t> &sink_1,
                 hls::stream<int16_t> &src_0, hls::stream<int16_t> &src_1,
                 int16_t shift, unsigned int packn)
{
ntt_layer:
    for (unsigned int i = 0; i < 128 * KYBER_K * packn; i++) {
        int16_t wing1 = src_0.read();
        int16_t wing2 = src_1.read();
        int16_t mask  = (1 << shift) - 1; // modulo 2 ^ shift
        int16_t index = (1 << shift) + (i & mask);

        ntt_butterfly(&wing1, &wing2, zetas[index]);

        if (i % 128 < 64) {
            sink_0 << wing1;
            sink_0 << wing2;
        }
        else {
            sink_1 << wing1;
            sink_1 << wing2;
        }
    }
}

template <typename T>
static void merge(hls::stream<T> &out, hls::stream<T> &in1,
                  hls::stream<T> &in2, int size, int split)
{
    int counter = 0;
merge:
    for (int i = 0; i < size; i++) {
        T t;
        if (counter < split) {
            t = in1.read();
            counter++;
        }
        else {
            t = in2.read();
            counter++;
            if (counter == (split << 1)) {
                counter = 0;
            }
        }
        out << t;
    }

}

static int16_t reduce(int16_t p)
{
        int16_t t;
        const int16_t v = ((1<<26) + KYBER_Q/2)/KYBER_Q;
        t  = ((int32_t)v*p + (1<<25)) >> 26;
        t *= KYBER_Q;
        return p - t;
}

static void bar_reduce(hls::stream<int16_t> &out, hls::stream<int16_t> &in,
            int size)
{
bar_reduce:
    for (unsigned int i = 0; i < size; i++) {
        out << reduce(in.read());
    }
}

static void repeat(hls::stream<int16_t> &out, hls::stream<int16_t> &in, unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
        int16_t tmp[256 * KYBER_K];
        for (unsigned int j = 0; j < 256 * KYBER_K; j++) {
            tmp[j] = in.read();
            out << tmp[j];
        }
        for (unsigned int i = 0; i < KYBER_K; i++) {
            for (unsigned int j = 0; j < 256 * KYBER_K; j++) {
                out << tmp[j];
            }
        }
    }
}

static void unpack(hls::stream<int16_t> &out1, hls::stream<ap_uint<256>> &out2,
            hls::stream<uint8_t> &in, unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
        for (unsigned int i = 0; i < KYBER_POLYVECBYTES; i += 3) {
                uint8_t t0 = in.read();
                uint8_t t1 = in.read();
                uint8_t t2 = in.read();
                out1 << (int16_t)(((t0 >> 0) | ((uint16_t)t1 << 8)) & 0xFFF);
                out1 << (int16_t)(((t1 >> 4) | ((uint16_t)t2 << 4)) & 0xFFF);
        }
        ap_uint<256> t256;
        uint8_t t8;
        for (unsigned int i = 0; i < KYBER_SYMBYTES; i++) {
            t8 = in.read();
            t256((i + 1) * 8 - 1, i * 8) = t8;
        }
        out2 << t256;
    }
}

#define XOF_BLOCKBYTES SHAKE128_RATE
#define GEN_MATRIX_NBLOCKS ((12*KYBER_N/8*(1 << 12)/KYBER_Q + XOF_BLOCKBYTES)/XOF_BLOCKBYTES)
static unsigned int rej_uniform(int16_t *r,
                         unsigned int len,
                         const uint8_t *buf,
                         unsigned int buflen)
{
  unsigned int ctr, pos;
  uint16_t val0, val1;

  ctr = pos = 0;
  while(ctr < len && pos + 3 <= buflen) {
    val0 = ((buf[pos+0] >> 0) | ((uint16_t)buf[pos+1] << 8)) & 0xFFF;
    val1 = ((buf[pos+1] >> 4) | ((uint16_t)buf[pos+2] << 4)) & 0xFFF;
    pos += 3;

    if(val0 < KYBER_Q)
      r[ctr++] = val0;
    if(ctr < len && val1 < KYBER_Q)
      r[ctr++] = val1;
  }

  return ctr;
}

static void gen_at_ext_read(uint8_t *extseed, hls::stream <ap_uint<256>> &in, bool act)
{
    if (act) {
        ap_uint<256> t = in.read();
        for (unsigned int i = 0; i < KYBER_SYMBYTES; i++) {
#pragma HLS UNROLL
            extseed[i] = t.range((i + 1) * 8 - 1, i * 8);
        }
    }
}

static void gen_at_ext_index(uint8_t *extseed, unsigned int i, unsigned int j)
{
    extseed[KYBER_SYMBYTES] = i;
    extseed[KYBER_SYMBYTES + 1] = j;
}

static void gen_at_absorb_init(uint64_t s[25], uint8_t *extseed, bool act)
{
    if (act) {
        uint8_t p = 0x1F;
        for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
            s[i] = 0;
        }

        unsigned int l;
        for(l = 0; l < KYBER_SYMBYTES + 2; l++) {
#pragma HLS UNROLL
            s[l / 8] ^= (uint64_t)extseed[l] << 8 * (l % 8);
        }

        s[l / 8] ^= (uint64_t)p << 8 * (l % 8);
        s[(SHAKE128_RATE - 1) / 8] ^= 1ULL << 63;
    }
}

static void gen_at_permute(uint64_t s1[25], bool act1, uint64_t s2[25], bool act2)
{
#pragma HLS INLINE OFF
#pragma HLS ALLOCATION function instances=KeccakF1600_StatePermute limit=1
    if (act1)
        KeccakF1600_StatePermute(s1);
    if (act2)
        KeccakF1600_StatePermute(s2);
}

static void gen_at_squeeze(hls::stream<int16_t> &out, uint8_t b[25][8],
                           unsigned int &ctr)
{
#pragma HLS INLINE OFF
    unsigned int i = 0;
    bool first = true;
    while (i < SHAKE128_RATE) {
        #pragma HLS PIPELINE II=2
        uint8_t b0 =  b[i / 8][i % 8];
        uint8_t b1 =  b[(i + 1) / 8][(i + 1) % 8];
        uint8_t b2 =  b[(i + 2) / 8][(i + 2) % 8];
        uint8_t b3 =  b[(i + 3) / 8][(i + 3) % 8];
        uint8_t b4 =  b[(i + 4) / 8][(i + 4) % 8];
        uint8_t b5 =  b[(i + 5) / 8][(i + 5) % 8];
        uint16_t val0 = ((b0 >> 0) | ((uint16_t)b1 << 8)) & 0xFFF;
        uint16_t val1 = ((b1 >> 4) | ((uint16_t)b2 << 4)) & 0xFFF;
        uint16_t val2 = ((b3 >> 0) | ((uint16_t)b4 << 8)) & 0xFFF;
        uint16_t val3 = ((b4 >> 4) | ((uint16_t)b5 << 4)) & 0xFFF;
        if (ctr == KYBER_N)
            break;
        if (first && val0 < KYBER_Q) {
            out << (int16_t)val0;
            ctr++;
            if (ctr < KYBER_N) {
                if (val1 < KYBER_Q) {
                    out << (int16_t)val1;
                    ctr++;
                    first = true;
                    if (val2 >= KYBER_Q && val3 >= KYBER_Q)
                        i += 6;
                    else
                        i += 3;
                }
                else if (i + 3 < SHAKE128_RATE && val2 < KYBER_Q) {
                    out << (int16_t)val2;
                    ctr++;
                    if (val3 >= KYBER_Q) {
                        i += 6;
                        first = true;
                    }
                    else {
                        i += 3;
                        first = false;
                    }
                }
                else if (i + 3 < SHAKE128_RATE && val3 < KYBER_Q) {
                    out << (int16_t)val3;
                    ctr++;
                    i += 6;
                    first = true;
                }
                else {
                    i += 6;
                    first = true; 
                }
            }
            else {
                first = true;
            }
        }
        else if (val1 < KYBER_Q) {
            out << (int16_t)val1;
            ctr++;
            if (ctr < KYBER_N) {
                if (i + 3 < SHAKE128_RATE && val2 < KYBER_Q) {
                    out << (int16_t)val2;
                    ctr++;
                    if (val3 >= KYBER_Q) {
                        i += 6;
                        first = true;
                    }
                    else {
                        i += 3;
                        first = false;
                    }
                }
                else if (i + 3 < SHAKE128_RATE && val3 < KYBER_Q) {
                    out << (int16_t)val3;
                    ctr++;
                    i += 6;
                    first = true;
                }
                else {
                    i += 6;
                    first = true; 
                }
            }
            else {
                first = true;
            }
        }
        else if (i + 3 < SHAKE128_RATE && val2 < KYBER_Q) {
            out << (int16_t)val2;
            ctr++;
            if (ctr < KYBER_N) {
                if (i + 3 < SHAKE128_RATE && val3 < KYBER_Q) {
                    out << (int16_t)val3;
                    ctr++;
                }
                i += 6;
                first = true;
            }
            else {
                first = true;
            }
        }
        else if (i + 3 < SHAKE128_RATE && val3 < KYBER_Q) {
            out << (int16_t)val3;
            ctr++;
            i += 6;
            first = true;
        }
        else {
            i += 6;
            first = true;
        }
    }
}

static void gen_at(hls::stream<int16_t> &out, hls::stream<int16_t> &in1, 
            hls::stream<ap_uint<256>> &in2, unsigned int packn)
{
    uint64_t s[25];
#pragma HLS ARRAY_PARTITION variable=s complete
    uint64_t s2[25];
#pragma HLS ARRAY_PARTITION variable=s2 complete
    uint8_t b[25][8];
#pragma HLS ARRAY_PARTITION variable=b dim=0 complete
    uint64_t snext[25];
#pragma HLS ARRAY_PARTITION variable=snext complete
    uint8_t extseed[KYBER_SYMBYTES + 2];
#pragma HLS ARRAY_PARTITION variable=extseed complete
    
    gen_at_ext_read(extseed, in2, true);
    gen_at_ext_index(extseed, 0, 0);
    gen_at_absorb_init(snext, extseed, true);
    gen_at_permute(s, false, snext, true);

    for (unsigned int pn = 0; pn < packn; pn++) {
        for(unsigned int i = 0; i < KYBER_K; i++) {
            for(unsigned int j = 0; j < KYBER_K; j++) {
                unsigned int ctr = 0;
                for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
                    s[i] = snext[i];
                }
                while (ctr < KYBER_N) {
                    for(unsigned int i = 0; i < 25; i++) {
#pragma HLS UNROLL
                        for (unsigned int j = 0; j < 8; j++)
#pragma HLS UNROLL
                            b[i][j] = s[i] >> 8 * (j % 8);
                    }
                    unsigned int nexti;
                    unsigned int nextj = j + 1;
                    if (nextj == KYBER_K)
                        nextj = 0;
                    if (nextj == 0)
                        nexti = i + 1;
                    else
                        nexti = i;
                    if (nexti == KYBER_K)
                        nexti = 0;
                    bool absorb = false;
                    if (ctr == 0)
                        absorb = true;
                    bool extupdate = false;
                    if (ctr == 0 && nexti == 0 && nextj == 0 && pn < packn - 1)
                        extupdate = true;
                    gen_at_squeeze(out, b, ctr);
                    gen_at_ext_read(extseed, in2, extupdate);
                    gen_at_ext_index(extseed, nexti, nextj);
                    gen_at_absorb_init(snext, extseed, absorb);
                    gen_at_permute(s, true, snext, absorb);
                }
            }
        }
        for (unsigned int i = 0; i < KYBER_K * KYBER_N; i++) {
            out << in1.read();
        }
    }
}

static int16_t fqmul(int16_t a, int16_t b)
{
    int32_t pr = (int32_t) a * b;
    int16_t t = (int16_t) pr * QINV;
    t = (pr - (int32_t) t * KYBER_Q) >> 16;
    return t;
}

static void basemul(int16_t r[2], const int16_t a[2], const int16_t b[2], int16_t zeta)
{
    r[0]  = fqmul(a[1], b[1]);
    r[0]  = fqmul(r[0], zeta);
    r[0] += fqmul(a[0], b[0]);
    r[1]  = fqmul(a[0], b[1]);
    r[1] += fqmul(a[1], b[0]);
}

static void basemul_montgomery(hls::stream<int16_t> &out, hls::stream<int16_t> &in1,
                        hls::stream<int16_t> &in2, unsigned int times)
{
    int16_t r[4], a[4], b[4];
basemul_montgomery:
    for (unsigned int i = 0; i < 64 * times; i++) {
        a[0] = in1.read();
        a[1] = in1.read();
        a[2] = in1.read();
        a[3] = in1.read();
        b[0] = in2.read();
        b[1] = in2.read();
        b[2] = in2.read();
        b[3] = in2.read();
        int16_t z = zetas[64 + i % 64];
        basemul(r, a, b, z);
        basemul(r + 2, a + 2, b + 2, -z);
        out << r[0];
        out << r[1];
        out << r[2];
        out << r[3];
    }
}

static void basemul_add(hls::stream<int16_t> &out, hls::stream<int16_t> &in, unsigned int times)
{
    for (unsigned int i = 0; i < times; i++) {
        int16_t tmp[256];
        for (unsigned int j = 0; j < KYBER_K; j++) {
            for (unsigned int k = 0; k < 256; k++) {
                int16_t t = in.read();
                if (j == 0) {
                    tmp[k] = t;
                }
                else if (j == KYBER_K - 1) {
                    out << tmp[k] + t;
                }
                else {
                    tmp[k] += t;
                }
            }
        }
    }
}

static void inv_butterfly(int16_t *x, int16_t *y, int16_t z)
{
    int16_t save = *x;
    //barrett reduce
    int16_t t;
    int16_t sum = *x + *y;
    const int16_t v = ((1<<26) + KYBER_Q/2)/KYBER_Q;
    t  = ((int32_t)v*sum + (1<<25)) >> 26;
    t *= KYBER_Q;
    *x = sum - t;

    *y -= save;
    *y = fqmul(z, *y);
}

static void invntt_layer(hls::stream<int16_t> &sink_0, hls::stream<int16_t> &sink_1,
                  hls::stream<int16_t> &src_0, hls::stream<int16_t> &src_1,
                  int16_t start, unsigned int times)
{
    unsigned int index = start;
invntt_layer:
    for (unsigned int i = 0; i < 128 * times; i++) {
        int16_t wing1, wing2;
        if ( i % 128 < 64) {
            wing1 = src_0.read();
            wing2 = src_0.read();
        }
        else {
            wing1 = src_1.read();
            wing2 = src_1.read();
        }

        inv_butterfly(&wing1, &wing2, zetas[index - 1]);
        
        sink_0 << wing1;
        sink_1 << wing2;
        index--;
        if (index == start / 2)
            index = start;
    }
}

static void stream_reverse_fqmul(hls::stream<int16_t> &out,
                                 hls::stream<int16_t> &in, unsigned int times)
{
stream_reverse_fqmul:
    for (unsigned int i = 0; i < 256 * times; i++) {
        int16_t c = in.read();
        out << fqmul(c, 1441);
    }
}

static void frommsg(hls::stream<int16_t> &out, hls::stream<uint8_t> &in,
             unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
        poly k;
        for (unsigned int i = 0; i < KYBER_INDCPA_MSGBYTES; i++) {
#if (KYBER_INDCPA_MSGBYTES != KYBER_N/8)
#error "KYBER_INDCPA_MSGBYTES must be equal to KYBER_N/8 bytes!"
#endif
            uint8_t m = in.read();
            for(unsigned int j = 0; j < 8; j++) {
                int16_t mask = -(int16_t)((m >> j)&1);
                out << (int16_t)(mask & ((KYBER_Q+1)/2));
            }
        }
    }
}

static void add_reduce(hls::stream<int16_t> &out, hls::stream<int16_t> &in1,
                hls::stream<int16_t> &in2, hls::stream<int16_t> &in3, unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
    add_reduce_1:
        for (unsigned int i = 0; i < 256 * KYBER_K; i++) {
            int16_t t = in1.read() + in2.read();
            out << reduce(t);
        }
    add_reduce_2:
        for (unsigned int i = 0; i < 256; i++) {
            int16_t t = in1.read() + in2.read() + in3.read();
            out << reduce(t);
        }
    }
}

static void compress(hls::stream<uint8_t> &out, hls::stream<int16_t> &in, unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
    uint8_t tmp;
#if (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 352))
    uint16_t t[8];
    for(unsigned int i = 0; i < KYBER_K; i++) {
        for(unsigned int j = 0; j < KYBER_N/8; j++) {
            #pragma HLS PIPELINE II=11
            for(unsigned int k = 0; k < 8; k++) {
                t[k]  = in.read();
                t[k] += ((int16_t)t[k] >> 15) & KYBER_Q;
                t[k]  = ((((uint32_t)t[k] << 11) + KYBER_Q/2)/KYBER_Q) & 0x7ff;
            }

            tmp = (t[0] >>  0);
            out << tmp;
            tmp = (t[0] >>  8) | (t[1] << 3);
            out << tmp;
            tmp = (t[1] >>  5) | (t[2] << 6);
            out << tmp;
            tmp = (t[2] >>  2);
            out << tmp;
            tmp = (t[2] >> 10) | (t[3] << 1);
            out << tmp;
            tmp = (t[3] >>  7) | (t[4] << 4);
            out << tmp;
            tmp = (t[4] >>  4) | (t[5] << 7);
            out << tmp;
            tmp = (t[5] >>  1);
            out << tmp;
            tmp = (t[5] >>  9) | (t[6] << 2);
            out << tmp;
            tmp = (t[6] >>  6) | (t[7] << 5);
            out << tmp;
            tmp = (t[7] >>  3);
            out << tmp;
        }
    }
#elif (KYBER_POLYVECCOMPRESSEDBYTES == (KYBER_K * 320))
    uint16_t t[4];
    for(unsigned int i = 0; i < KYBER_K; i++) {
        for(unsigned int j = 0; j < KYBER_N/4; j++) {
            #pragma HLS PIPELINE II=5
            for(unsigned int k = 0; k < 4; k++) {
                t[k]  = in.read();
                t[k] += ((int16_t)t[k] >> 15) & KYBER_Q;
                t[k]  = ((((uint32_t)t[k] << 10) + KYBER_Q/2)/ KYBER_Q) & 0x3ff;
            }

            tmp = (t[0] >> 0);
            out << tmp;
            tmp = (t[0] >> 8) | (t[1] << 2);
            out << tmp;
            tmp = (t[1] >> 6) | (t[2] << 4);
            out << tmp;
            tmp = (t[2] >> 4) | (t[3] << 6);
            out << tmp;
            tmp = (t[3] >> 2);
            out << tmp;
        }
    }
#else
#error "KYBER_POLYVECCOMPRESSEDBYTES needs to be in {320*KYBER_K, 352*KYBER_K}"
#endif

  int16_t u;
  uint8_t t2[8];

#if (KYBER_POLYCOMPRESSEDBYTES == 128)
for(unsigned int i = 0; i < KYBER_N/8; i++) {
    #pragma HLS PIPELINE II=8
    for(unsigned int j = 0; j < 8; j++) {
        u  = in.read();
        u += (u >> 15) & KYBER_Q;
        t2[j] = ((((uint16_t)u << 4) + KYBER_Q/2)/KYBER_Q) & 15;
    }

    tmp =  t2[0] | (t2[1] << 4);
    out << tmp;
    tmp =  t2[2] | (t2[3] << 4);
    out << tmp;
    tmp =  t2[4] | (t2[5] << 4);
    out << tmp;
    tmp =  t2[6] | (t2[7] << 4);
    out << tmp;
}
#elif (KYBER_POLYCOMPRESSEDBYTES == 160)
for(unsigned int i = 0; i < KYBER_N/8; i++) {
    #pragma HLS PIPELINE II=8
    for(unsigned int j = 0; j < 8; j++) {
        u  = in.read();
        u += (u >> 15) & KYBER_Q;
        t2[j] = ((((uint32_t)u << 5) + KYBER_Q/2)/KYBER_Q) & 31;
    }

    tmp = (t2[0] >> 0) | (t2[1] << 5);
    out << tmp;
    tmp = (t2[1] >> 3) | (t2[2] << 2) | (t2[3] << 7);
    out << tmp;
    tmp = (t2[3] >> 1) | (t2[4] << 4);
    out << tmp;
    tmp = (t2[4] >> 4) | (t2[5] << 1) | (t2[6] << 6);
    out << tmp;
    tmp = (t2[6] >> 2) | (t2[7] << 3);
    out << tmp;
}
#else
#error "KYBER_POLYCOMPRESSEDBYTES needs to be in {128, 160}"
#endif
    }
}

static void verify(hls::stream<int> &out, hls::stream<uint8_t> &in1,
                   hls::stream<uint8_t> &in2, unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
        uint8_t r = 0;
        int v;
        for (unsigned int i = 0; i < KYBER_CIPHERTEXTBYTES; i++) {
            r |= in1.read() ^ in2.read();
        }
        v = (-(uint64_t)r) >> 63;
        out << v;
    }
}

static void cmov(hls::stream<uint8_t> &out, hls::stream<uint8_t> &in1,
                 hls::stream<uint8_t> &in2, hls::stream<int> &in3, unsigned int packn)
{
    for (unsigned int p = 0; p < packn; p++) {
        int t = in3.read();
        uint8_t b = -t;
        for (unsigned int i = 0; i < KYBER_SYMBYTES; i++) {
            uint8_t tmp;
            uint8_t t1 = in1.read();
            uint8_t t2 = in2.read();
            tmp = t1 ^ (b & (t1 ^ t2));
            out << tmp;
        }
    }
}

static void writemem(uint8_t *out, hls::stream<uint8_t> &stream, int size)
{
writemem:
    for (int i = 0; i < size; i++) {
        out[i] = stream.read();
    }
}

extern "C" {
void k_kem_enc(uint8_t *ct, uint8_t *ss, uint8_t *buf, uint8_t *pk,
               unsigned int packn)
{
    //#pragma HLS INTERFACE m_axi port=ct  offset=slave bundle=gmemct
    //#pragma HLS INTERFACE m_axi port=ss  offset=slave bundle=gmemss
    //#pragma HLS INTERFACE m_axi port=buf offset=slave bundle=gmembuf
    //#pragma HLS INTERFACE m_axi port=pk  offset=slave bundle=gmempk
    static hls::stream<int16_t> s_ntt_init("s_ntt_init");
    #pragma HLS STREAM variable=s_ntt_init depth=128
    static hls::stream<int16_t> snttl_7_0("sntt_l_7_0");
    #pragma HLS STREAM variable=snttl_7_0 depth=128
    static hls::stream<int16_t> snttl_7_1("sntt_l_7_1");
    #pragma HLS STREAM variable=snttl_7_1 depth=128
    static hls::stream<int16_t> snttl_6_0("sntt_l_6_0");
    #pragma HLS STREAM variable=snttl_6_0 depth=128
    static hls::stream<int16_t> snttl_6_1("sntt_l_6_1");
    #pragma HLS STREAM variable=snttl_6_1 depth=128
    static hls::stream<int16_t> snttl_5_0("sntt_l_5_0");
    #pragma HLS STREAM variable=snttl_5_0 depth=128
    static hls::stream<int16_t> snttl_5_1("sntt_l_5_1");
    #pragma HLS STREAM variable=snttl_5_1 depth=128
    static hls::stream<int16_t> snttl_4_0("sntt_l_4_0");
    #pragma HLS STREAM variable=snttl_4_0 depth=128
    static hls::stream<int16_t> snttl_4_1("sntt_l_4_1");
    #pragma HLS STREAM variable=snttl_4_1 depth=128
    static hls::stream<int16_t> snttl_3_0("sntt_l_3_0");
    #pragma HLS STREAM variable=snttl_3_0 depth=128
    static hls::stream<int16_t> snttl_3_1("sntt_l_3_1");
    #pragma HLS STREAM variable=snttl_3_1 depth=128
    static hls::stream<int16_t> snttl_2_0("sntt_l_2_0");
    #pragma HLS STREAM variable=snttl_2_0 depth=128
    static hls::stream<int16_t> snttl_2_1("sntt_l_2_1");
    #pragma HLS STREAM variable=snttl_2_1 depth=128
    static hls::stream<int16_t> snttl_1_0("sntt_l_1_0");
    #pragma HLS STREAM variable=snttl_1_0 depth=128
    static hls::stream<int16_t> snttl_1_1("sntt_l_1_1");
    #pragma HLS STREAM variable=snttl_1_1 depth=128
    static hls::stream<int16_t> snttl_0_0("sntt_l_0_0");
    #pragma HLS STREAM variable=snttl_0_0 depth=128
    static hls::stream<int16_t> snttl_0_1("sntt_l_0_1");
    #pragma HLS STREAM variable=snttl_0_1 depth=128
    static hls::stream<int16_t> s_ntt_out("s_ntt_out");
    #pragma HLS STREAM variable=s_ntt_out depth=512
    static hls::stream<int16_t> s_ntt_red("s_ntt_red");
    #pragma HLS STREAM variable=s_ntt_red depth=512
    static hls::stream<int16_t> s_at("s_at");
    #pragma HLS STREAM variable=s_at depth=256*kyber*(kyber + 1)*2
    static hls::stream<int16_t> s_sp("s_sp");
    #pragma HLS STREAM variable=s_sp depth=256*kyber*(kyber + 1)*2
    static hls::stream<int16_t> s_ep("s_ep");
    #pragma HLS STREAM variable=s_ep depth=6*256*(kyber+1)
    static hls::stream<int16_t> s_k("s_k");
    #pragma HLS STREAM variable=s_k depth=256
    static hls::stream<int16_t> s_b("s_b");
    #pragma HLS STREAM variable=s_b depth=256*(kyber+1)
    static hls::stream<uint8_t> s_ct("s_ct");
    #pragma HLS STREAM variable=s_b depth=bytes
    static hls::stream<int16_t> s_basemul_out("s_basemul_out");
    #pragma HLS STREAM variable=s_basemul_out depth=128
    static hls::stream<int16_t> s_basemul_add("s_basemul_add");
    #pragma HLS STREAM variable=s_basemul_add depth=128
    static hls::stream<int16_t> sinvl_0_0("sinvl_0_0");
    #pragma HLS STREAM variable=sinvl_0_0 depth=128
    static hls::stream<int16_t> sinvl_0_1("sinvl_0_1");
    #pragma HLS STREAM variable=sinvl_0_1 depth=128
    static hls::stream<int16_t> sinvl_1_0("sinvl_1_280");
    #pragma HLS STREAM variable=sinvl_1_0 depth=128
    static hls::stream<int16_t> sinvl_1_1("sinvl_1_1");
    #pragma HLS STREAM variable=sinvl_1_1 depth=128
    static hls::stream<int16_t> sinvl_2_0("sinvl_2_0");
    #pragma HLS STREAM variable=sinvl_2_0 depth=128
    static hls::stream<int16_t> sinvl_2_1("sinvl_2_1");
    #pragma HLS STREAM variable=sinvl_2_1 depth=128
    static hls::stream<int16_t> sinvl_3_0("sinvl_3_0");
    #pragma HLS STREAM variable=sinvl_3_0 depth=128
    static hls::stream<int16_t> sinvl_3_1("sinvl_3_1");
    #pragma HLS STREAM variable=sinvl_3_1 depth=128
    static hls::stream<int16_t> sinvl_4_0("sinvl_4_0");
    #pragma HLS STREAM variable=sinvl_4_0 depth=128
    static hls::stream<int16_t> sinvl_4_1("sinvl_4_1");
    #pragma HLS STREAM variable=sinvl_4_1 depth=128
    static hls::stream<int16_t> sinvl_5_0("sinvl_5_0");
    #pragma HLS STREAM variable=sinvl_5_0 depth=128
    static hls::stream<int16_t> sinvl_5_1("sinvl_5_1");
    #pragma HLS STREAM variable=sinvl_5_1 depth=128
    static hls::stream<int16_t> sinvl_6_0("sinvl_6_0");
    #pragma HLS STREAM variable=sinvl_6_0 depth=128
    static hls::stream<int16_t> sinvl_6_1("sinvl_6_1");
    #pragma HLS STREAM variable=sinvl_6_1 depth=128
    static hls::stream<int16_t> sinvl_7_0("sinvl_7_0");
    #pragma HLS STREAM variable=sinvl_7_0 depth=128
    static hls::stream<int16_t> sinvl_7_1("sinvl_7_1");
    #pragma HLS STREAM variable=sinvl_7_1 depth=128
    static hls::stream<int16_t> s_inv_out("s_inv_out");
    #pragma HLS STREAM variable=s_inv_out depth=512
    static hls::stream<int16_t> s_inv_merge("s_inv_merge");
    #pragma HLS STREAM variable=s_inv_merge depth=128
    static hls::stream<uint8_t> s_coins_orig("s_coins_orig");
    #pragma HLS STREAM variable=s_coins_orig depth=symbytes
    static hls::stream<ap_uint<256>> s_coins("s_coins");
    #pragma HLS STREAM variable=s_coins depth=4
    static hls::stream<ap_uint<264>> s_key("s_key");
    #pragma HLS STREAM variable=s_key depth=4
    static hls::stream<uint8_t> s_shake_sp("s_shake_sp");
    #pragma HLS STREAM variable=s_shake_sp depth=50
    static hls::stream<uint8_t> s_shake_ep("s_shake_ep");
    #pragma HLS STREAM variable=s_shake_ep depth=6*eta2*kybern/4
    static hls::stream<uint8_t> s_m("s_m");
    #pragma HLS STREAM variable=s_m depth=200
    static hls::stream<uint8_t> s_pk("s_pk");
    #pragma HLS STREAM variable=s_pk depth=200
    static hls::stream<ap_uint<256>> s_seed("s_seed");
    #pragma HLS STREAM variable=s_seed depth=4
    static hls::stream<int16_t> s_pkpv("s_pkpv");
    #pragma HLS STREAM variable=s_pkpv depth=256*kyber*6
    // Doubling s_pkpv in order to save the 1 more cycle
    // needed to to produce 2 values (3 cycles needed)
    // That way 2 loops are always available and thus
    // synbytes can be read immediately in gen_at
    static hls::stream<uint8_t> s_m_orig("s_m_orig");
    #pragma HLS STREAM variable=s_m_orig depth=2*symbytes
    static hls::stream<uint8_t> s_pk_orig("s_pk_orig");
    #pragma HLS STREAM variable=s_pk_orig depth=200
    static hls::stream<uint8_t> s_pk_sha("s_pk_sha");
    #pragma HLS STREAM variable=s_pk_sha depth=1600
    static hls::stream<uint8_t> s_m_sha("s_m_sha");
    #pragma HLS STREAM variable=s_m_sha depth=200
    static hls::stream<uint8_t> s_m_Hpk("s_m_Hpk");
    #pragma HLS STREAM variable=s_m_Hpk depth=200
    static hls::stream<uint8_t> s_kr_orig("s_kr_orig");
    #pragma HLS STREAM variable=s_kr_orig depth=2*2*symbytes
    static hls::stream<uint8_t> s_kr("s_kr");
    #pragma HLS STREAM variable=s_kr depth=2*2*symbytes
    static hls::stream<uint8_t> s_krl("s_krl");
    #pragma HLS STREAM variable=s_krl depth=4*symbytes
    static hls::stream<uint8_t> s_krh("s_krh");
    #pragma HLS STREAM variable=s_krh depth=4*symbytes
    static hls::stream<uint8_t> s_ct_orig("s_orig");
    #pragma HLS STREAM variable=s_ct_orig depth=200
    static hls::stream<uint8_t> s_ct_sha("s_ct_sha");
    #pragma HLS STREAM variable=s_ct_sha depth=200
    static hls::stream<uint8_t> s_ss("s_ss");
    #pragma HLS STREAM variable=s_ss depth=200
    static hls::stream<ap_uint<512>> s_kr_bundled("s_kr_bundled");
    #pragma HLS STREAM variable=s_kr_bundled depth=4
    //static hls::stream<uint8_t> s_inter("s_inter");
    //#pragma HLS STREAM variable=s_inter depth=50
    #pragma HLS DATAFLOW

    readmem(s_m_orig, buf, packn * KYBER_SYMBYTES);
    readmem(s_pk_orig, pk, packn * KYBER_PUBLICKEYBYTES);

    duplicate(s_m_sha, s_m, s_m_orig, packn * KYBER_SYMBYTES);
    duplicate(s_pk_sha, s_pk, s_pk_orig, packn * KYBER_PUBLICKEYBYTES);

    kyber_sha(s_m_Hpk, s_m_sha, s_pk_sha, packn);
    sha(s_kr_orig, 2 * KYBER_SYMBYTES,
                   s_m_Hpk, 2 * KYBER_SYMBYTES,
                   SHA3_512_RATE, packn);
    split(s_krl, s_coins_orig, s_kr_orig, packn * 2 * KYBER_SYMBYTES, KYBER_SYMBYTES);
    make_coins(s_coins, s_coins_orig, packn);
    // NTT
    get_key(s_key, s_coins, packn);

    kyber_shake(s_shake_sp, KYBER_ETA1 * KYBER_N / 4,
                s_shake_ep, KYBER_ETA2 * KYBER_N / 4,
                s_key, packn);
    // KYBER_ETA1 has to be {2,3}
#if KYBER_ETA1 == 2
    cbd2(s_ntt_init, s_shake_sp, packn * KYBER_K);
#else
    cbd3(s_ntt_init, s_shake_sp, packn * KYBER_K);
#endif
    cbd2(s_ep, s_shake_ep, packn * (KYBER_K + 1));

    split(snttl_7_0, snttl_7_1, s_ntt_init, 256 * KYBER_K * packn, 128);
    ntt_layer(snttl_6_0, snttl_6_1, snttl_7_0, snttl_7_1, 0, packn);
    ntt_layer(snttl_5_0, snttl_5_1, snttl_6_0, snttl_6_1, 1, packn);
    ntt_layer(snttl_4_0, snttl_4_1, snttl_5_0, snttl_5_1, 2, packn);
    ntt_layer(snttl_3_0, snttl_3_1, snttl_4_0, snttl_4_1, 3, packn);
    ntt_layer(snttl_2_0, snttl_2_1, snttl_3_0, snttl_3_1, 4, packn);
    ntt_layer(snttl_1_0, snttl_1_1, snttl_2_0, snttl_2_1, 5, packn);
    ntt_layer(snttl_0_0, snttl_0_1, snttl_1_0, snttl_1_1, 6, packn);
    merge(s_ntt_out, snttl_0_0, snttl_0_1, 256 * KYBER_K * packn, 1);
    bar_reduce(s_ntt_red, s_ntt_out, 256 * KYBER_K * packn);
    repeat(s_sp, s_ntt_red, packn);

    // Basemul Montgomery
    unpack(s_pkpv, s_seed, s_pk, packn);
    gen_at(s_at, s_pkpv, s_seed, packn);
    basemul_montgomery(s_basemul_out, s_sp, s_at, KYBER_K * (KYBER_K + 1) * packn);
    basemul_add(s_basemul_add, s_basemul_out, (KYBER_K + 1) * packn);

    // InvNTT
    split(sinvl_0_0, sinvl_0_1, s_basemul_add, 256 * (KYBER_K + 1) * packn, 1);
    invntt_layer(sinvl_1_0, sinvl_1_1, sinvl_0_0, sinvl_0_1, 128, (KYBER_K + 1) * packn);
    invntt_layer(sinvl_2_0, sinvl_2_1, sinvl_1_0, sinvl_1_1, 64, (KYBER_K + 1) * packn);
    invntt_layer(sinvl_3_0, sinvl_3_1, sinvl_2_0, sinvl_2_1, 32, (KYBER_K + 1) * packn);
    invntt_layer(sinvl_4_0, sinvl_4_1, sinvl_3_0, sinvl_3_1, 16, (KYBER_K + 1) * packn);
    invntt_layer(sinvl_5_0, sinvl_5_1, sinvl_4_0, sinvl_4_1, 8, (KYBER_K + 1) * packn);
    invntt_layer(sinvl_6_0, sinvl_6_1, sinvl_5_0, sinvl_5_1, 4, (KYBER_K + 1) * packn);
    invntt_layer(sinvl_7_0, sinvl_7_1, sinvl_6_0, sinvl_6_1, 2, (KYBER_K + 1) * packn);
    merge(s_inv_merge, sinvl_7_0, sinvl_7_1, 256 * (KYBER_K + 1) * packn, 128);
    stream_reverse_fqmul(s_inv_out, s_inv_merge, (KYBER_K + 1) * packn);

    // Add and reduce
    frommsg(s_k, s_m, packn);
    add_reduce(s_b, s_inv_out, s_ep, s_k, packn);

    //compress
    compress(s_ct_orig, s_b, packn);
    //duplicate(s_ct_sha, s_ct, s_ct_orig, packn * KYBER_CIPHERTEXTBYTES);
    //sha(s_krh, KYBER_SYMBYTES, s_ct_sha, KYBER_CIPHERTEXTBYTES, SHA3_256_RATE, packn);
    //merge(s_kr, s_krl, s_krh, packn * 2 * KYBER_SYMBYTES, KYBER_SYMBYTES);
    //make_kr(s_kr_bundled, s_kr, packn);

    //shake256(s_ss, KYBER_SSBYTES, s_kr_bundled, packn);

    writemem(ct, s_ct_orig, KYBER_CIPHERTEXTBYTES * packn);
    writemem(ss, s_krl, KYBER_SSBYTES * packn);
}
}

extern "C" {
void k_kem_dec(uint8_t *ss, uint8_t *ct, uint8_t *sk, unsigned int packn)
{
    //#pragma HLS INTERFACE m_axi port=ss  offset=slave bundle=gmemss
    //#pragma HLS INTERFACE m_axi port=ct  offset=slave bundle=gmemct
    //#pragma HLS INTERFACE m_axi port=sk  offset=slave bundle=gmemsk
    static hls::stream<int16_t> s_d_ntt_init("s_d_ntt_init");
    #pragma HLS STREAM variable=s_d_ntt_init depth=128
    static hls::stream<int16_t> s_d_nttl_7_0("s_d_ntt_l_7_0");
    #pragma HLS STREAM variable=s_d_nttl_7_0 depth=128
    static hls::stream<int16_t> s_d_nttl_7_1("s_d_ntt_l_7_1");
    #pragma HLS STREAM variable=s_d_nttl_7_1 depth=128
    static hls::stream<int16_t> s_d_nttl_6_0("s_d_ntt_l_6_0");
    #pragma HLS STREAM variable=s_d_nttl_6_0 depth=128
    static hls::stream<int16_t> s_d_nttl_6_1("s_d_ntt_l_6_1");
    #pragma HLS STREAM variable=s_d_nttl_6_1 depth=128
    static hls::stream<int16_t> s_d_nttl_5_0("s_d_ntt_l_5_0");
    #pragma HLS STREAM variable=s_d_nttl_5_0 depth=128
    static hls::stream<int16_t> s_d_nttl_5_1("s_d_ntt_l_5_1");
    #pragma HLS STREAM variable=s_d_nttl_5_1 depth=128
    static hls::stream<int16_t> s_d_nttl_4_0("s_d_ntt_l_4_0");
    #pragma HLS STREAM variable=s_d_nttl_4_0 depth=128
    static hls::stream<int16_t> s_d_nttl_4_1("s_d_ntt_l_4_1");
    #pragma HLS STREAM variable=s_d_nttl_4_1 depth=128
    static hls::stream<int16_t> s_d_nttl_3_0("s_d_ntt_l_3_0");
    #pragma HLS STREAM variable=s_d_nttl_3_0 depth=128
    static hls::stream<int16_t> s_d_nttl_3_1("s_d_ntt_l_3_1");
    #pragma HLS STREAM variable=s_d_nttl_3_1 depth=128
    static hls::stream<int16_t> s_d_nttl_2_0("s_d_ntt_l_2_0");
    #pragma HLS STREAM variable=s_d_nttl_2_0 depth=128
    static hls::stream<int16_t> s_d_nttl_2_1("s_d_ntt_l_2_1");
    #pragma HLS STREAM variable=s_d_nttl_2_1 depth=128
    static hls::stream<int16_t> s_d_nttl_1_0("s_d_ntt_l_1_0");
    #pragma HLS STREAM variable=s_d_nttl_1_0 depth=128
    static hls::stream<int16_t> s_d_nttl_1_1("s_d_ntt_l_1_1");
    #pragma HLS STREAM variable=s_d_nttl_1_1 depth=128
    static hls::stream<int16_t> s_d_nttl_0_0("s_d_ntt_l_0_0");
    #pragma HLS STREAM variable=s_d_nttl_0_0 depth=128
    static hls::stream<int16_t> s_d_nttl_0_1("s_d_ntt_l_0_1");
    #pragma HLS STREAM variable=s_d_nttl_0_1 depth=128
    static hls::stream<int16_t> s_d_ntt_out("s_d_ntt_out");
    #pragma HLS STREAM variable=s_d_ntt_out depth=512
    static hls::stream<int16_t> s_d_ntt_red("s_d_ntt_red");
    #pragma HLS STREAM variable=s_d_ntt_red depth=512
    static hls::stream<int16_t> s_ntt_init("s_ntt_init");
    #pragma HLS STREAM variable=s_ntt_init depth=128
    static hls::stream<int16_t> snttl_7_0("sntt_l_7_0");
    #pragma HLS STREAM variable=snttl_7_0 depth=128
    static hls::stream<int16_t> snttl_7_1("sntt_l_7_1");
    #pragma HLS STREAM variable=snttl_7_1 depth=128
    static hls::stream<int16_t> snttl_6_0("sntt_l_6_0");
    #pragma HLS STREAM variable=snttl_6_0 depth=128
    static hls::stream<int16_t> snttl_6_1("sntt_l_6_1");
    #pragma HLS STREAM variable=snttl_6_1 depth=128
    static hls::stream<int16_t> snttl_5_0("sntt_l_5_0");
    #pragma HLS STREAM variable=snttl_5_0 depth=128
    static hls::stream<int16_t> snttl_5_1("sntt_l_5_1");
    #pragma HLS STREAM variable=snttl_5_1 depth=128
    static hls::stream<int16_t> snttl_4_0("sntt_l_4_0");
    #pragma HLS STREAM variable=snttl_4_0 depth=128
    static hls::stream<int16_t> snttl_4_1("sntt_l_4_1");
    #pragma HLS STREAM variable=snttl_4_1 depth=128
    static hls::stream<int16_t> snttl_3_0("sntt_l_3_0");
    #pragma HLS STREAM variable=snttl_3_0 depth=128
    static hls::stream<int16_t> snttl_3_1("sntt_l_3_1");
    #pragma HLS STREAM variable=snttl_3_1 depth=128
    static hls::stream<int16_t> snttl_2_0("sntt_l_2_0");
    #pragma HLS STREAM variable=snttl_2_0 depth=128
    static hls::stream<int16_t> snttl_2_1("sntt_l_2_1");
    #pragma HLS STREAM variable=snttl_2_1 depth=128
    static hls::stream<int16_t> snttl_1_0("sntt_l_1_0");
    #pragma HLS STREAM variable=snttl_1_0 depth=128
    static hls::stream<int16_t> snttl_1_1("sntt_l_1_1");
    #pragma HLS STREAM variable=snttl_1_1 depth=128
    static hls::stream<int16_t> snttl_0_0("sntt_l_0_0");
    #pragma HLS STREAM variable=snttl_0_0 depth=128
    static hls::stream<int16_t> snttl_0_1("sntt_l_0_1");
    #pragma HLS STREAM variable=snttl_0_1 depth=128
    static hls::stream<int16_t> s_ntt_out("s_ntt_out");
    #pragma HLS STREAM variable=s_ntt_out depth=512
    static hls::stream<int16_t> s_ntt_red("s_ntt_red");
    #pragma HLS STREAM variable=s_ntt_red depth=512
    static hls::stream<int16_t> s_at("s_at");
    #pragma HLS STREAM variable=s_at depth=256*kyber*(kyber + 1)*2
    static hls::stream<int16_t> s_sp("s_sp");
    #pragma HLS STREAM variable=s_sp depth=256*kyber*(kyber + 1)*2
    static hls::stream<int16_t> s_ep("s_ep");
    #pragma HLS STREAM variable=s_ep depth=6*256*(kyber+1)
    static hls::stream<int16_t> s_k("s_k");
    #pragma HLS STREAM variable=s_k depth=256
    static hls::stream<int16_t> s_b("s_b");
    #pragma HLS STREAM variable=s_b depth=256*(kyber+1)
    static hls::stream<uint8_t> s_ct("s_ct");
    #pragma HLS STREAM variable=s_b depth=bytes
    static hls::stream<int16_t> s_d_basemul_out("s_d_basemul_out");
    #pragma HLS STREAM variable=s_d_basemul_out depth=128
    static hls::stream<int16_t> s_d_basemul_add("s_d_basemul_add");
    #pragma HLS STREAM variable=s_d_basemul_add depth=128
    static hls::stream<int16_t> s_d_invl_0_0("s_d_invl_0_0");
    #pragma HLS STREAM variable=s_d_invl_0_0 depth=128
    static hls::stream<int16_t> s_d_invl_0_1("s_d_invl_0_1");
    #pragma HLS STREAM variable=s_d_invl_0_1 depth=128
    static hls::stream<int16_t> s_d_invl_1_0("s_d_invl_1_280");
    #pragma HLS STREAM variable=s_d_invl_1_0 depth=128
    static hls::stream<int16_t> s_d_invl_1_1("s_d_invl_1_1");
    #pragma HLS STREAM variable=s_d_invl_1_1 depth=128
    static hls::stream<int16_t> s_d_invl_2_0("s_d_invl_2_0");
    #pragma HLS STREAM variable=s_d_invl_2_0 depth=128
    static hls::stream<int16_t> s_d_invl_2_1("s_d_invl_2_1");
    #pragma HLS STREAM variable=s_d_invl_2_1 depth=128
    static hls::stream<int16_t> s_d_invl_3_0("s_d_invl_3_0");
    #pragma HLS STREAM variable=s_d_invl_3_0 depth=128
    static hls::stream<int16_t> s_d_invl_3_1("s_d_invl_3_1");
    #pragma HLS STREAM variable=s_d_invl_3_1 depth=128
    static hls::stream<int16_t> s_d_invl_4_0("s_d_invl_4_0");
    #pragma HLS STREAM variable=s_d_invl_4_0 depth=128
    static hls::stream<int16_t> s_d_invl_4_1("s_d_invl_4_1");
    #pragma HLS STREAM variable=s_d_invl_4_1 depth=128
    static hls::stream<int16_t> s_d_invl_5_0("s_d_invl_5_0");
    #pragma HLS STREAM variable=s_d_invl_5_0 depth=128
    static hls::stream<int16_t> s_d_invl_5_1("s_d_invl_5_1");
    #pragma HLS STREAM variable=s_d_invl_5_1 depth=128
    static hls::stream<int16_t> s_d_invl_6_0("s_d_invl_6_0");
    #pragma HLS STREAM variable=s_d_invl_6_0 depth=128
    static hls::stream<int16_t> s_d_invl_6_1("s_d_invl_6_1");
    #pragma HLS STREAM variable=s_d_invl_6_1 depth=128
    static hls::stream<int16_t> s_d_invl_7_0("s_d_invl_7_0");
    #pragma HLS STREAM variable=s_d_invl_7_0 depth=128
    static hls::stream<int16_t> s_d_invl_7_1("s_d_invl_7_1");
    #pragma HLS STREAM variable=s_d_invl_7_1 depth=128
    static hls::stream<int16_t> s_d_inv_out("s_d_inv_out");
    #pragma HLS STREAM variable=s_d_inv_out depth=512
    static hls::stream<int16_t> s_d_inv_merge("s_d_inv_merge");
    #pragma HLS STREAM variable=s_d_inv_merge depth=128
    static hls::stream<int16_t> s_basemul_out("s_basemul_out");
    #pragma HLS STREAM variable=s_basemul_out depth=128
    static hls::stream<int16_t> s_basemul_add("s_basemul_add");
    #pragma HLS STREAM variable=s_basemul_add depth=128
    static hls::stream<int16_t> sinvl_0_0("sinvl_0_0");
    #pragma HLS STREAM variable=sinvl_0_0 depth=128
    static hls::stream<int16_t> sinvl_0_1("sinvl_0_1");
    #pragma HLS STREAM variable=sinvl_0_1 depth=128
    static hls::stream<int16_t> sinvl_1_0("sinvl_1_280");
    #pragma HLS STREAM variable=sinvl_1_0 depth=128
    static hls::stream<int16_t> sinvl_1_1("sinvl_1_1");
    #pragma HLS STREAM variable=sinvl_1_1 depth=128
    static hls::stream<int16_t> sinvl_2_0("sinvl_2_0");
    #pragma HLS STREAM variable=sinvl_2_0 depth=128
    static hls::stream<int16_t> sinvl_2_1("sinvl_2_1");
    #pragma HLS STREAM variable=sinvl_2_1 depth=128
    static hls::stream<int16_t> sinvl_3_0("sinvl_3_0");
    #pragma HLS STREAM variable=sinvl_3_0 depth=128
    static hls::stream<int16_t> sinvl_3_1("sinvl_3_1");
    #pragma HLS STREAM variable=sinvl_3_1 depth=128
    static hls::stream<int16_t> sinvl_4_0("sinvl_4_0");
    #pragma HLS STREAM variable=sinvl_4_0 depth=128
    static hls::stream<int16_t> sinvl_4_1("sinvl_4_1");
    #pragma HLS STREAM variable=sinvl_4_1 depth=128
    static hls::stream<int16_t> sinvl_5_0("sinvl_5_0");
    #pragma HLS STREAM variable=sinvl_5_0 depth=128
    static hls::stream<int16_t> sinvl_5_1("sinvl_5_1");
    #pragma HLS STREAM variable=sinvl_5_1 depth=128
    static hls::stream<int16_t> sinvl_6_0("sinvl_6_0");
    #pragma HLS STREAM variable=sinvl_6_0 depth=128
    static hls::stream<int16_t> sinvl_6_1("sinvl_6_1");
    #pragma HLS STREAM variable=sinvl_6_1 depth=128
    static hls::stream<int16_t> sinvl_7_0("sinvl_7_0");
    #pragma HLS STREAM variable=sinvl_7_0 depth=128
    static hls::stream<int16_t> sinvl_7_1("sinvl_7_1");
    #pragma HLS STREAM variable=sinvl_7_1 depth=128
    static hls::stream<int16_t> s_inv_out("s_inv_out");
    #pragma HLS STREAM variable=s_inv_out depth=512
    static hls::stream<int16_t> s_inv_merge("s_inv_merge");
    #pragma HLS STREAM variable=s_inv_merge depth=128
    static hls::stream<uint8_t> s_coins_orig("s_coins_orig");
    #pragma HLS STREAM variable=s_coins_orig depth=symbytes
    static hls::stream<ap_uint<256>> s_coins("s_coins");
    #pragma HLS STREAM variable=s_coins depth=4
    static hls::stream<ap_uint<264>> s_key("s_key");
    #pragma HLS STREAM variable=s_key depth=4
    static hls::stream<uint8_t> s_shake_sp("s_shake_sp");
    #pragma HLS STREAM variable=s_shake_sp depth=50
    static hls::stream<uint8_t> s_shake_ep("s_shake_ep");
    #pragma HLS STREAM variable=s_shake_ep depth=6*eta2*kybern/4
    static hls::stream<uint8_t> s_m("s_m");
    #pragma HLS STREAM variable=s_m depth=200
    static hls::stream<uint8_t> s_pk("s_pk");
    #pragma HLS STREAM variable=s_pk depth=200
    static hls::stream<ap_uint<256>> s_seed("s_seed");
    #pragma HLS STREAM variable=s_seed depth=4
    static hls::stream<int16_t> s_pkpv("s_pkpv");
    #pragma HLS STREAM variable=s_pkpv depth=256*kyber*6
    // Doubling s_pkpv in order to save the 1 more cycle
    // needed to to produce 2 values (3 cycles needed)
    // That way 2 loops are always available and thus
    // synbytes can be read immediately in gen_at
    static hls::stream<uint8_t> s_buf("s_buf");
    #pragma HLS STREAM variable=s_buf depth=2*symbytes
    static hls::stream<uint8_t> s_pk_orig("s_pk_orig");
    #pragma HLS STREAM variable=s_pk_orig depth=200
    static hls::stream<uint8_t> s_pk_sha("s_pk_sha");
    #pragma HLS STREAM variable=s_pk_sha depth=1600
    static hls::stream<uint8_t> s_buf_sha("s_buf_sha");
    #pragma HLS STREAM variable=s_buf_sha depth=200
    static hls::stream<uint8_t> s_kr_orig("s_kr_orig");
    #pragma HLS STREAM variable=s_kr_orig depth=2*2*symbytes
    //static hls::stream<uint8_t> s_kr("s_kr");
    //#pragma HLS STREAM variable=s_kr depth=2*2*symbytes
    static hls::stream<uint8_t> s_krl("s_krl");
    #pragma HLS STREAM variable=s_krl depth=4*symbytes
    static hls::stream<uint8_t> s_krh("s_krh");
    #pragma HLS STREAM variable=s_krh depth=4*symbytes
    static hls::stream<uint8_t> s_ct_orig("s_orig");
    #pragma HLS STREAM variable=s_ct_orig depth=200
    static hls::stream<uint8_t> s_ct_sha("s_ct_sha");
    #pragma HLS STREAM variable=s_ct_sha depth=200
    static hls::stream<uint8_t> s_ss("s_ss");
    #pragma HLS STREAM variable=s_ss depth=200
    static hls::stream<uint8_t> s_ss_mov("s_ss_mov");
    #pragma HLS STREAM variable=s_ss_mov depth=200
    static hls::stream<ap_uint<512>> s_kr_bundled("s_kr_bundled");
    #pragma HLS STREAM variable=s_kr_bundled depth=4
    //kem_dec specific
    static hls::stream<uint8_t> s_sk_orig("s_sk_orig");
    #pragma HLS STREAM variable=s_sk_orig depth=4*skbytes
    static hls::stream<uint8_t> s_d_sk("s_d_sk");
    #pragma HLS STREAM variable=s_d_sk depth=4*skbytes
    static hls::stream<uint8_t> s_kr_ver("s_kr_ver");
    #pragma HLS STREAM variable=s_kr_ver depth=200
    static hls::stream<uint8_t> s_buf_h("s_buf_h");
    #pragma HLS STREAM variable=s_buf_h depth=200
    static hls::stream<uint8_t> s_ct_0("s_0");
    #pragma HLS STREAM variable=s_ct_0 depth=4*bytes
    static hls::stream<uint8_t> s_ct_1("s_1");
    #pragma HLS STREAM variable=s_ct_1 depth=12*bytes
    static hls::stream<uint8_t> s_ct_2("s_2");
    #pragma HLS STREAM variable=s_ct_2 depth=4*bytes
    static hls::stream<int16_t> s_d_b("s_d_b");
    #pragma HLS STREAM variable=s_d_b depth=1000
    static hls::stream<int16_t> s_d_v("s_d_v");
    #pragma HLS STREAM variable=s_d_v depth=2000
    static hls::stream<int16_t> s_sub("s_sub");
    #pragma HLS STREAM variable=s_sub depth=1000
    static hls::stream<int16_t> s_skpv("s_skpv");
    #pragma HLS STREAM variable=s_skpv depth=2*kyber*kybern
    static hls::stream<int16_t> s_mp("s_mp");
    #pragma HLS STREAM variable=s_mp depth=200
    static hls::stream<uint8_t> s_buf_l("s_buf_l");
    #pragma HLS STREAM variable=s_buf_l depth=200
    static hls::stream<uint8_t> s_buf_l_0("s_buf_l_0");
    #pragma HLS STREAM variable=s_buf_l_0 depth=200
    static hls::stream<uint8_t> s_buf_l_1("s_buf_l_1");
    #pragma HLS STREAM variable=s_buf_l_1 depth=200
    static hls::stream<uint8_t> s_cmp("s_cmp");
    #pragma HLS STREAM variable=s_cmp depth=4*bytes
    static hls::stream<int> s_fail("s_fail");
    #pragma HLS STREAM variable=s_fail depth=200
    //static hls::stream<uint8_t> s_krl_mov("s_krl_mov");
    //#pragma HLS STREAM variable=s_krl_mov depth=4*symbytes

    #pragma HLS DATAFLOW
    
    readmem(s_ct, ct, packn * KYBER_CIPHERTEXTBYTES);
    readmem(s_sk_orig, sk, packn * KYBER_SECRETKEYBYTES);
    sk_split(s_d_sk, s_pk, s_buf_h, s_kr_ver, s_sk_orig, packn);
    
    triple(s_ct_0, s_ct_1, s_ct_2, s_ct, packn * KYBER_CIPHERTEXTBYTES);
    decompress(s_d_b, s_d_v, s_ct_0, packn);
    frombytes(s_skpv, s_d_sk, packn);
    
    split(s_d_nttl_7_0, s_d_nttl_7_1, s_d_b, 256 * KYBER_K * packn, 128);
    ntt_layer(s_d_nttl_6_0, s_d_nttl_6_1, s_d_nttl_7_0, s_d_nttl_7_1, 0, packn);
    ntt_layer(s_d_nttl_5_0, s_d_nttl_5_1, s_d_nttl_6_0, s_d_nttl_6_1, 1, packn);
    ntt_layer(s_d_nttl_4_0, s_d_nttl_4_1, s_d_nttl_5_0, s_d_nttl_5_1, 2, packn);
    ntt_layer(s_d_nttl_3_0, s_d_nttl_3_1, s_d_nttl_4_0, s_d_nttl_4_1, 3, packn);
    ntt_layer(s_d_nttl_2_0, s_d_nttl_2_1, s_d_nttl_3_0, s_d_nttl_3_1, 4, packn);
    ntt_layer(s_d_nttl_1_0, s_d_nttl_1_1, s_d_nttl_2_0, s_d_nttl_2_1, 5, packn);
    ntt_layer(s_d_nttl_0_0, s_d_nttl_0_1, s_d_nttl_1_0, s_d_nttl_1_1, 6, packn);
    merge(s_d_ntt_out, s_d_nttl_0_0, s_d_nttl_0_1, 256 * KYBER_K * packn, 1);
    bar_reduce(s_d_ntt_red, s_d_ntt_out, 256 * KYBER_K * packn);

    basemul_montgomery(s_d_basemul_out, s_skpv, s_d_ntt_red, packn * KYBER_K);
    basemul_add(s_mp, s_d_basemul_out, packn);

    // InvNTT
    split(s_d_invl_0_0, s_d_invl_0_1, s_mp, 256 * packn, 1);
    invntt_layer(s_d_invl_1_0, s_d_invl_1_1, s_d_invl_0_0, s_d_invl_0_1, 128, packn);
    invntt_layer(s_d_invl_2_0, s_d_invl_2_1, s_d_invl_1_0, s_d_invl_1_1, 64, packn);
    invntt_layer(s_d_invl_3_0, s_d_invl_3_1, s_d_invl_2_0, s_d_invl_2_1, 32, packn);
    invntt_layer(s_d_invl_4_0, s_d_invl_4_1, s_d_invl_3_0, s_d_invl_3_1, 16, packn);
    invntt_layer(s_d_invl_5_0, s_d_invl_5_1, s_d_invl_4_0, s_d_invl_4_1, 8, packn);
    invntt_layer(s_d_invl_6_0, s_d_invl_6_1, s_d_invl_5_0, s_d_invl_5_1, 4, packn);
    invntt_layer(s_d_invl_7_0, s_d_invl_7_1, s_d_invl_6_0, s_d_invl_6_1, 2, packn);
    merge(s_d_inv_merge, s_d_invl_7_0, s_d_invl_7_1, 256 * packn, 128);
    stream_reverse_fqmul(s_d_inv_out, s_d_inv_merge, packn);

    sub_reduce(s_sub, s_d_v, s_d_inv_out, packn);
    tomsg(s_buf_l, s_sub, packn);
    duplicate(s_buf_l_0, s_buf_l_1, s_buf_l, packn * KYBER_SYMBYTES);

    merge(s_buf_sha, s_buf_l_0, s_buf_h, packn * 2 * KYBER_SYMBYTES, KYBER_SYMBYTES);

    sha(s_kr_orig, 2 * KYBER_SYMBYTES,
                   s_buf_sha, 2 * KYBER_SYMBYTES,
                   SHA3_512_RATE, packn);
    split(s_krl, s_coins_orig, s_kr_orig, packn * 2 * KYBER_SYMBYTES, KYBER_SYMBYTES);
    
    /*
     * indcpa_enc start
     */
    make_coins(s_coins, s_coins_orig, packn);
    // NTT
    get_key(s_key, s_coins, packn);

    kyber_shake(s_shake_sp, KYBER_ETA1 * KYBER_N / 4,
                s_shake_ep, KYBER_ETA2 * KYBER_N / 4,
                s_key, packn);
    // KYBER_ETA1 has to be {2,3}
#if KYBER_ETA1 == 2
    cbd2(s_ntt_init, s_shake_sp, packn * KYBER_K);
#else
    cbd3(s_ntt_init, s_shake_sp, packn * KYBER_K);
#endif
    cbd2(s_ep, s_shake_ep, packn * (KYBER_K + 1));

    split(snttl_7_0, snttl_7_1, s_ntt_init, 256 * KYBER_K * packn, 128);
    ntt_layer(snttl_6_0, snttl_6_1, snttl_7_0, snttl_7_1, 0, packn);
    ntt_layer(snttl_5_0, snttl_5_1, snttl_6_0, snttl_6_1, 1, packn);
    ntt_layer(snttl_4_0, snttl_4_1, snttl_5_0, snttl_5_1, 2, packn);
    ntt_layer(snttl_3_0, snttl_3_1, snttl_4_0, snttl_4_1, 3, packn);
    ntt_layer(snttl_2_0, snttl_2_1, snttl_3_0, snttl_3_1, 4, packn);
    ntt_layer(snttl_1_0, snttl_1_1, snttl_2_0, snttl_2_1, 5, packn);
    ntt_layer(snttl_0_0, snttl_0_1, snttl_1_0, snttl_1_1, 6, packn);
    merge(s_ntt_out, snttl_0_0, snttl_0_1, 256 * KYBER_K * packn, 1);
    bar_reduce(s_ntt_red, s_ntt_out, 256 * KYBER_K * packn);
    repeat(s_sp, s_ntt_red, packn);

    // Basemul Montgomery
    unpack(s_pkpv, s_seed, s_pk, packn);
    gen_at(s_at, s_pkpv, s_seed, packn);
    basemul_montgomery(s_basemul_out, s_sp, s_at, packn * KYBER_K * (KYBER_K + 1));
    basemul_add(s_basemul_add, s_basemul_out, packn * (KYBER_K + 1));

    // InvNTT
    split(sinvl_0_0, sinvl_0_1, s_basemul_add, 256 * (KYBER_K + 1) * packn, 1);
    invntt_layer(sinvl_1_0, sinvl_1_1, sinvl_0_0, sinvl_0_1, 128, packn * (KYBER_K + 1));
    invntt_layer(sinvl_2_0, sinvl_2_1, sinvl_1_0, sinvl_1_1, 64, packn * (KYBER_K + 1));
    invntt_layer(sinvl_3_0, sinvl_3_1, sinvl_2_0, sinvl_2_1, 32, packn * (KYBER_K + 1));
    invntt_layer(sinvl_4_0, sinvl_4_1, sinvl_3_0, sinvl_3_1, 16, packn * (KYBER_K + 1));
    invntt_layer(sinvl_5_0, sinvl_5_1, sinvl_4_0, sinvl_4_1, 8, packn * (KYBER_K + 1));
    invntt_layer(sinvl_6_0, sinvl_6_1, sinvl_5_0, sinvl_5_1, 4, packn * (KYBER_K + 1));
    invntt_layer(sinvl_7_0, sinvl_7_1, sinvl_6_0, sinvl_6_1, 2, packn * (KYBER_K + 1));
    merge(s_inv_merge, sinvl_7_0, sinvl_7_1, 256 * (KYBER_K + 1) * packn, 128);
    stream_reverse_fqmul(s_inv_out, s_inv_merge, packn * (KYBER_K + 1));

    // Add and reduce
    frommsg(s_k, s_buf_l_1, packn);
    add_reduce(s_b, s_inv_out, s_ep, s_k, packn);

    //compress
    compress(s_cmp, s_b, packn);
    /*
     * indcpa_enc end
     */
    verify(s_fail, s_ct_1, s_cmp, packn);

    //sha(s_krh, KYBER_SYMBYTES, s_ct_2, KYBER_CIPHERTEXTBYTES, SHA3_256_RATE, packn);
    //cmov(s_krl_mov, s_krl, s_kr_ver, s_fail, packn);
    //merge(s_kr, s_krl_mov, s_krh, packn * 2 * KYBER_SYMBYTES, KYBER_SYMBYTES);
    //make_kr(s_kr_bundled, s_kr, packn);

    //shake256(s_ss, KYBER_SSBYTES, s_kr_bundled, packn);
    rkprf(s_ss, s_kr_ver, s_ct_2, packn);

    cmov(s_ss_mov, s_krl, s_ss, s_fail, packn);

    writemem(ss, s_ss_mov, KYBER_SSBYTES * packn);
}
}

void mlkem_accelerator(unsigned char kem_cfg, uint8_t *ct_in, uint8_t *ct_out, uint8_t *ss_enc_out, uint8_t *ss_dec_out, uint8_t *buf_in, uint8_t *pk_in, uint8_t *sk_in){

    #pragma HLS INTERFACE m_axi port=ct_in      offset=slave bundle=gmemct
    #pragma HLS INTERFACE m_axi port=ct_out     offset=slave bundle=gmemct
    #pragma HLS INTERFACE m_axi port=ss_enc_out offset=slave bundle=gmemss
    #pragma HLS INTERFACE m_axi port=ss_dec_out offset=slave bundle=gmemss
    #pragma HLS INTERFACE m_axi port=buf_in     offset=slave bundle=gmembuf
    #pragma HLS INTERFACE m_axi port=pk_in      offset=slave bundle=gmempk
	#pragma HLS INTERFACE m_axi port=sk_in      offset=slave bundle=gmemsk

    #pragma HLS INTERFACE s_axilite port=kem_cfg    bundle=control
    #pragma HLS INTERFACE s_axilite port=ct_in      bundle=control
    #pragma HLS INTERFACE s_axilite port=ct_out     bundle=control
    #pragma HLS INTERFACE s_axilite port=ss_enc_out bundle=control
    #pragma HLS INTERFACE s_axilite port=ss_dec_out bundle=control
    #pragma HLS INTERFACE s_axilite port=buf_in     bundle=control
    #pragma HLS INTERFACE s_axilite port=pk_in      bundle=control
    #pragma HLS INTERFACE s_axilite port=sk_in      bundle=control
    #pragma HLS INTERFACE s_axilite port=return     bundle=control

    if(kem_cfg==0){
        //kem_cfg=0 equals to encryption kernel
        k_kem_enc(ct_out, ss_enc_out, buf_in, pk_in, 1);
    } else {
        //kem_cfg=<any other number than 0> equals to decrytion kernel
        k_kem_dec(ss_dec_out, ct_in, sk_in, 1);
    }
}
