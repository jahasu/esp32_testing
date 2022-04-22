#include "sine_lut.h"

static uint32_t sine_lut_1k[] = {
    0x0,0x10b5,0x2120,0x30fb,0x3fff,0x4deb,0x5a82,0x658c,
    0x6ed9,0x7641,0x7ba2,0x7ee7,0x7fff,0x7ee7,0x7ba2,0x7641,
    0x6ed9,0x658c,0x5a82,0x4deb,0x3fff,0x30fb,0x2120,0x10b5,
    0x0,0xef4a,0xdedf,0xcf04,0xc000,0xb214,0xa57d,0x9a73,
    0x9126,0x89be,0x845d,0x8118,0x8000,0x8118,0x845d,0x89be,
    0x9126,0x9a73,0xa57d,0xb214,0xc000,0xcf04,0xdedf,0xef4a
};


static uint32_t sine_lut_1k3[] = {
    0x0,0x15a1,0x2aa4,0x3e6c,0x5069,0x6015,0x6cfe,0x76c4,
    0x7d20,0x7fe1,0x7ef6,0x7a63,0x724a,0x66e8,0x5891,0x47ac,
    0x34b8,0x2040,0xada,0xf525,0xdfbf,0xcb47,0xb853,0xa76e,
    0x9917,0x8db5,0x859c,0x8109,0x801e,0x82df,0x893b,0x9301,
    0x9fea,0xaf96,0xc193,0xd55b,0xea5e
};


static uint32_t sine_lut_5k3[] = {
    0x0,0x5246,0x7e0d,0x6ed9,0x2bc7,0xd438,0x9126,0x81f2,
    0xadb9
};


static uint32_t sine_lut_440[] = {
    0x0,0x75f,0xeb9,0x1606,0x1d40,0x2462,0x2b64,0x3242,
    0x38f4,0x3f77,0x45c3,0x4bd4,0x51a5,0x5730,0x5c71,0x6163,
    0x6603,0x6a4c,0x6e3a,0x71cb,0x74fb,0x77c7,0x7a2e,0x7c2d,
    0x7dc2,0x7eec,0x7faa,0x7ffc,0x7fe0,0x7f58,0x7e64,0x7d04,
    0x7b3a,0x7907,0x766e,0x736f,0x700f,0x6c4f,0x6833,0x63be,
    0x5ef4,0x59da,0x5473,0x4ec5,0x48d3,0x42a4,0x3c3c,0x35a1,
    0x2ed8,0x27e7,0x20d4,0x19a6,0x1261,0xb0d,0x3b0,0xfc4f,
    0xf4f2,0xed9e,0xe659,0xdf2b,0xd818,0xd127,0xca5e,0xc3c3,
    0xbd5b,0xb72c,0xb13a,0xab8c,0xa625,0xa10b,0x9c41,0x97cc,
    0x93b0,0x8ff0,0x8c90,0x8991,0x86f8,0x84c5,0x82fb,0x819b,
    0x80a7,0x801f,0x8003,0x8055,0x8113,0x823d,0x83d2,0x85d1,
    0x8838,0x8b04,0x8e34,0x91c5,0x95b3,0x99fc,0x9e9c,0xa38e,
    0xa8cf,0xae5a,0xb42b,0xba3c,0xc088,0xc70b,0xcdbd,0xd49b,
    0xdb9d,0xe2bf,0xe9f9,0xf146,0xf8a0
};


static uint32_t sine_lut_552[] = {
    0x0,0x93c,0x126c,0x1b84,0x2476,0x2d38,0x35be,0x3dfc,
    0x45e8,0x4d76,0x549c,0x5b52,0x618e,0x6747,0x6c77,0x7116,
    0x751e,0x788a,0x7b55,0x7d7b,0x7efa,0x7fcf,0x7ffa,0x7f7a,
    0x7e4f,0x7c7d,0x7a04,0x76e8,0x732d,0x6ed9,0x69f1,0x647b,
    0x5e80,0x5806,0x5116,0x49bb,0x41fd,0x39e7,0x3184,0x28de,
    0x2002,0x16fc,0xdd6,0x49e,0xfb61,0xf229,0xe903,0xdffd,
    0xd721,0xce7b,0xc618,0xbe02,0xb644,0xaee9,0xa7f9,0xa17f,
    0x9b84,0x960e,0x9126,0x8cd2,0x8917,0x85fb,0x8382,0x81b0,
    0x8085,0x8005,0x8030,0x8105,0x8284,0x84aa,0x8775,0x8ae1,
    0x8ee9,0x9388,0x98b8,0x9e71,0xa4ad,0xab63,0xb289,0xba17,
    0xc203,0xca41,0xd2c7,0xdb89,0xe47b,0xed93,0xf6c3
};

static uint32_t sine_lut_60[] = {
    0x0,0x101,0x202,0x303,0x405,0x506,0x607,0x708,
    0x809,0x90a,0xa0a,0xb0b,0xc0b,0xd0b,0xe0b,0xf0b,
    0x100a,0x110a,0x1208,0x1307,0x1405,0x1503,0x1601,0x16ff,
    0x17fc,0x18f8,0x19f4,0x1af0,0x1bec,0x1ce6,0x1de1,0x1edb,
    0x1fd4,0x20cd,0x21c6,0x22be,0x23b5,0x24ac,0x25a2,0x2698,
    0x278d,0x2882,0x2975,0x2a69,0x2b5b,0x2c4d,0x2d3e,0x2e2e,
    0x2f1e,0x300d,0x30fb,0x31e8,0x32d5,0x33c1,0x34ac,0x3596,
    0x367f,0x3768,0x384f,0x3936,0x3a1c,0x3b00,0x3be4,0x3cc7,
    0x3da9,0x3e8a,0x3f6a,0x4049,0x4128,0x4205,0x42e0,0x43bb,
    0x4495,0x456e,0x4646,0x471c,0x47f2,0x48c6,0x4999,0x4a6b,
    0x4b3c,0x4c0b,0x4cda,0x4da7,0x4e73,0x4f3e,0x5007,0x50cf,
    0x5196,0x525c,0x5320,0x53e3,0x54a5,0x5565,0x5624,0x56e2,
    0x579e,0x5859,0x5913,0x59cb,0x5a82,0x5b37,0x5beb,0x5c9d,
    0x5d4e,0x5dfd,0x5eab,0x5f58,0x6003,0x60ac,0x6154,0x61fa,
    0x629f,0x6343,0x63e4,0x6484,0x6523,0x65c0,0x665b,0x66f5,
    0x678d,0x6823,0x68b8,0x694b,0x69dd,0x6a6d,0x6afb,0x6b87,
    0x6c12,0x6c9b,0x6d22,0x6da8,0x6e2c,0x6eae,0x6f2e,0x6fad,
    0x702a,0x70a5,0x711e,0x7196,0x720c,0x727f,0x72f2,0x7362,
    0x73d0,0x743d,0x74a8,0x7511,0x7578,0x75dd,0x7641,0x76a2,
    0x7702,0x7760,0x77bc,0x7816,0x786e,0x78c4,0x7918,0x796b,
    0x79bb,0x7a0a,0x7a56,0x7aa1,0x7aea,0x7b31,0x7b76,0x7bb9,
    0x7bfa,0x7c39,0x7c76,0x7cb1,0x7cea,0x7d21,0x7d56,0x7d89,
    0x7dbb,0x7dea,0x7e17,0x7e42,0x7e6c,0x7e93,0x7eb8,0x7edb,
    0x7efd,0x7f1c,0x7f39,0x7f54,0x7f6e,0x7f85,0x7f9a,0x7fad,
    0x7fbe,0x7fcd,0x7fdb,0x7fe6,0x7fef,0x7ff6,0x7ffb,0x7ffe,
    0x7fff,0x7ffe,0x7ffb,0x7ff6,0x7fef,0x7fe6,0x7fdb,0x7fcd,
    0x7fbe,0x7fad,0x7f9a,0x7f85,0x7f6e,0x7f54,0x7f39,0x7f1c,
    0x7efd,0x7edb,0x7eb8,0x7e93,0x7e6c,0x7e42,0x7e17,0x7dea,
    0x7dbb,0x7d89,0x7d56,0x7d21,0x7cea,0x7cb1,0x7c76,0x7c39,
    0x7bfa,0x7bb9,0x7b76,0x7b31,0x7aea,0x7aa1,0x7a56,0x7a0a,
    0x79bb,0x796b,0x7918,0x78c4,0x786e,0x7816,0x77bc,0x7760,
    0x7702,0x76a2,0x7641,0x75dd,0x7578,0x7511,0x74a8,0x743d,
    0x73d0,0x7362,0x72f2,0x727f,0x720c,0x7196,0x711e,0x70a5,
    0x702a,0x6fad,0x6f2e,0x6eae,0x6e2c,0x6da8,0x6d22,0x6c9b,
    0x6c12,0x6b87,0x6afb,0x6a6d,0x69dd,0x694b,0x68b8,0x6823,
    0x678d,0x66f5,0x665b,0x65c0,0x6523,0x6484,0x63e4,0x6343,
    0x629f,0x61fa,0x6154,0x60ac,0x6003,0x5f58,0x5eab,0x5dfd,
    0x5d4e,0x5c9d,0x5beb,0x5b37,0x5a82,0x59cb,0x5913,0x5859,
    0x579e,0x56e2,0x5624,0x5565,0x54a5,0x53e3,0x5320,0x525c,
    0x5196,0x50cf,0x5007,0x4f3e,0x4e73,0x4da7,0x4cda,0x4c0b,
    0x4b3c,0x4a6b,0x4999,0x48c6,0x47f2,0x471c,0x4646,0x456e,
    0x4495,0x43bb,0x42e0,0x4205,0x4128,0x4049,0x3f6a,0x3e8a,
    0x3da9,0x3cc7,0x3be4,0x3b00,0x3a1c,0x3936,0x384f,0x3768,
    0x367f,0x3596,0x34ac,0x33c1,0x32d5,0x31e8,0x30fb,0x300d,
    0x2f1e,0x2e2e,0x2d3e,0x2c4d,0x2b5b,0x2a69,0x2975,0x2882,
    0x278d,0x2698,0x25a2,0x24ac,0x23b5,0x22be,0x21c6,0x20cd,
    0x1fd4,0x1edb,0x1de1,0x1ce6,0x1bec,0x1af0,0x19f4,0x18f8,
    0x17fc,0x16ff,0x1601,0x1503,0x1405,0x1307,0x1208,0x110a,
    0x100a,0xf0b,0xe0b,0xd0b,0xc0b,0xb0b,0xa0a,0x90a,
    0x809,0x708,0x607,0x506,0x405,0x303,0x202,0x101,
    0x0,0xfefe,0xfdfd,0xfcfc,0xfbfa,0xfaf9,0xf9f8,0xf8f7,
    0xf7f6,0xf6f5,0xf5f5,0xf4f4,0xf3f4,0xf2f4,0xf1f4,0xf0f4,
    0xeff5,0xeef5,0xedf7,0xecf8,0xebfa,0xeafc,0xe9fe,0xe900,
    0xe803,0xe707,0xe60b,0xe50f,0xe413,0xe319,0xe21e,0xe124,
    0xe02b,0xdf32,0xde39,0xdd41,0xdc4a,0xdb53,0xda5d,0xd967,
    0xd872,0xd77d,0xd68a,0xd596,0xd4a4,0xd3b2,0xd2c1,0xd1d1,
    0xd0e1,0xcff2,0xcf04,0xce17,0xcd2a,0xcc3e,0xcb53,0xca69,
    0xc980,0xc897,0xc7b0,0xc6c9,0xc5e3,0xc4ff,0xc41b,0xc338,
    0xc256,0xc175,0xc095,0xbfb6,0xbed7,0xbdfa,0xbd1f,0xbc44,
    0xbb6a,0xba91,0xb9b9,0xb8e3,0xb80d,0xb739,0xb666,0xb594,
    0xb4c3,0xb3f4,0xb325,0xb258,0xb18c,0xb0c1,0xaff8,0xaf30,
    0xae69,0xada3,0xacdf,0xac1c,0xab5a,0xaa9a,0xa9db,0xa91d,
    0xa861,0xa7a6,0xa6ec,0xa634,0xa57d,0xa4c8,0xa414,0xa362,
    0xa2b1,0xa202,0xa154,0xa0a7,0x9ffc,0x9f53,0x9eab,0x9e05,
    0x9d60,0x9cbc,0x9c1b,0x9b7b,0x9adc,0x9a3f,0x99a4,0x990a,
    0x9872,0x97dc,0x9747,0x96b4,0x9622,0x9592,0x9504,0x9478,
    0x93ed,0x9364,0x92dd,0x9257,0x91d3,0x9151,0x90d1,0x9052,
    0x8fd5,0x8f5a,0x8ee1,0x8e69,0x8df3,0x8d80,0x8d0d,0x8c9d,
    0x8c2f,0x8bc2,0x8b57,0x8aee,0x8a87,0x8a22,0x89be,0x895d,
    0x88fd,0x889f,0x8843,0x87e9,0x8791,0x873b,0x86e7,0x8694,
    0x8644,0x85f5,0x85a9,0x855e,0x8515,0x84ce,0x8489,0x8446,
    0x8405,0x83c6,0x8389,0x834e,0x8315,0x82de,0x82a9,0x8276,
    0x8244,0x8215,0x81e8,0x81bd,0x8193,0x816c,0x8147,0x8124,
    0x8102,0x80e3,0x80c6,0x80ab,0x8091,0x807a,0x8065,0x8052,
    0x8041,0x8032,0x8024,0x8019,0x8010,0x8009,0x8004,0x8001,
    0x8000,0x8001,0x8004,0x8009,0x8010,0x8019,0x8024,0x8032,
    0x8041,0x8052,0x8065,0x807a,0x8091,0x80ab,0x80c6,0x80e3,
    0x8102,0x8124,0x8147,0x816c,0x8193,0x81bd,0x81e8,0x8215,
    0x8244,0x8276,0x82a9,0x82de,0x8315,0x834e,0x8389,0x83c6,
    0x8405,0x8446,0x8489,0x84ce,0x8515,0x855e,0x85a9,0x85f5,
    0x8644,0x8694,0x86e7,0x873b,0x8791,0x87e9,0x8843,0x889f,
    0x88fd,0x895d,0x89be,0x8a22,0x8a87,0x8aee,0x8b57,0x8bc2,
    0x8c2f,0x8c9d,0x8d0d,0x8d80,0x8df3,0x8e69,0x8ee1,0x8f5a,
    0x8fd5,0x9052,0x90d1,0x9151,0x91d3,0x9257,0x92dd,0x9364,
    0x93ed,0x9478,0x9504,0x9592,0x9622,0x96b4,0x9747,0x97dc,
    0x9872,0x990a,0x99a4,0x9a3f,0x9adc,0x9b7b,0x9c1b,0x9cbc,
    0x9d60,0x9e05,0x9eab,0x9f53,0x9ffc,0xa0a7,0xa154,0xa202,
    0xa2b1,0xa362,0xa414,0xa4c8,0xa57d,0xa634,0xa6ec,0xa7a6,
    0xa861,0xa91d,0xa9db,0xaa9a,0xab5a,0xac1c,0xacdf,0xada3,
    0xae69,0xaf30,0xaff8,0xb0c1,0xb18c,0xb258,0xb325,0xb3f4,
    0xb4c3,0xb594,0xb666,0xb739,0xb80d,0xb8e3,0xb9b9,0xba91,
    0xbb6a,0xbc44,0xbd1f,0xbdfa,0xbed7,0xbfb6,0xc095,0xc175,
    0xc256,0xc338,0xc41b,0xc4ff,0xc5e3,0xc6c9,0xc7b0,0xc897,
    0xc980,0xca69,0xcb53,0xcc3e,0xcd2a,0xce17,0xcf04,0xcff2,
    0xd0e1,0xd1d1,0xd2c1,0xd3b2,0xd4a4,0xd596,0xd68a,0xd77d,
    0xd872,0xd967,0xda5d,0xdb53,0xdc4a,0xdd41,0xde39,0xdf32,
    0xe02b,0xe124,0xe21e,0xe319,0xe413,0xe50f,0xe60b,0xe707,
    0xe803,0xe900,0xe9fe,0xeafc,0xebfa,0xecf8,0xedf7,0xeef5,
    0xeff5,0xf0f4,0xf1f4,0xf2f4,0xf3f4,0xf4f4,0xf5f5,0xf6f5,
    0xf7f6,0xf8f7,0xf9f8,0xfaf9,0xfbfa,0xfcfc,0xfdfd,0xfefe,
};


static uint32_t sine_lut_TEST_A[] = {
    0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,0x8888,0x9999,
    0xaaaa,0xbbbb,0xcccc,0xdddd,0xeeee,0xffff,
};

static uint32_t sine_lut_TEST_B[] = {
    0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,0x1111,
    0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,0x2222,
    0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,0x3333,
    0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,0x4444,
    0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,0x5555,
    0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,0x6666,
    0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,0x7777,
};


static uint32_t sine_lut_TEST_A_24[] = {
    0x111111,0x222222,0x333333,0x444444,0x555555,0x666666,0x777777,0x888888,0x999999,
    0xaaaaaa,0xbbbbbb,0xcccccc,0xdddddd,0xeeeeee,0xffffff,
};


/*
Reference a sine LUT and get size
*/
void lut_gen_init(lut_gen_t *lsg, lut_freq_e freq)
{
    switch (freq) {
        case LUT_FREQ_1K:
            lsg->lut = sine_lut_1k;
            lsg->lut_size = sizeof(sine_lut_1k)/4;
            break;
        case LUT_FREQ_1K3:
            lsg->lut = sine_lut_1k3;
            lsg->lut_size = sizeof(sine_lut_1k3)/4;
            break;
        case LUT_FREQ_440:
            lsg->lut = sine_lut_440;
            lsg->lut_size = sizeof(sine_lut_440)/4;
            break;
        case LUT_FREQ_552:
            lsg->lut = sine_lut_552;
            lsg->lut_size = sizeof(sine_lut_552)/4;
            break;
        case LUT_FREQ_5K3:
            lsg->lut = sine_lut_5k3;
            lsg->lut_size = sizeof(sine_lut_5k3)/4;
            break;
        case LUT_FREQ_60:
            lsg->lut = sine_lut_60;
            lsg->lut_size = sizeof(sine_lut_60)/4;
            break;
        case LUT_TEST_A:
            lsg->lut = sine_lut_TEST_A;
            lsg->lut_size = sizeof(sine_lut_TEST_A)/4;
            break;
        case LUT_TEST_B:
            lsg->lut = sine_lut_TEST_B;
            lsg->lut_size = sizeof(sine_lut_TEST_B)/4;
            break;
        case LUT_TEST_A_24:
            lsg->lut = sine_lut_TEST_A_24;
            lsg->lut_size = sizeof(sine_lut_TEST_A_24)/4;
            break;
        
        default:
            lsg->lut = sine_lut_1k;
            lsg->lut_size = sizeof(sine_lut_1k)/4;
            break;
    }
    lsg->pos = 0;
}


/*
Returns one sample
*/
uint32_t lut_gen_get_sample(lut_gen_t *lsg)
{
    uint32_t sample = lsg->lut[lsg->pos];
    if(lsg->pos < lsg->lut_size-1)
        lsg->pos++;
    else
        lsg->pos = 0;
    return sample;
}


/*
Returns a number of samples
*/
void lut_gen_get_samples(lut_gen_t *lsg, uint32_t *out_buffer, size_t samples)
{
    for(int i=0; i<samples; i++) {
        out_buffer[i] = lut_gen_get_sample(lsg);
    }
}