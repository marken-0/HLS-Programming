#include <stdint.h>

#define leftShift(v, n) ( v<<n | v>>(32-n))

#define quarterRound(x, a, b, c, d) \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 16); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 12); \
  x[a] += x[b]; x[d] = leftShift(x[d] ^ x[a], 8); \
  x[c] += x[d]; x[b] = leftShift(x[b] ^ x[c], 7);

void chacha20_block(uint32_t state[16], uint32_t out[16]) {
    int i;
    for (i = 0; i < 16; ++i)
        out[i] = state[i];
    for (i = 0; i < 20; i += 2) {
        quarterRound(out, 0, 4, 8, 12)
        quarterRound(out, 1, 5, 9, 13)
        quarterRound(out, 2, 6, 10, 14)
        quarterRound(out, 3, 7, 11, 15)
        quarterRound(out, 0, 5, 10, 15)
        quarterRound(out, 1, 6, 11, 12)
        quarterRound(out, 2, 7, 8, 13)
        quarterRound(out, 3, 4, 9, 14)
    }
    for (i = 0; i < 16; ++i)
        out[i] += state[i];
}

void chacha20(uint8_t key[32], uint8_t nonce[12], uint32_t counter, uint8_t *plaintext, uint8_t *ciphertext, uint32_t len) {
    uint32_t state[16], block[16], keystream[64];
    int i, j;

    state[0] = 0x61707865; state[1] = 0x3320646e; state[2] = 0x79622d32; state[3] = 0x6b206574;
    for (i = 0; i < 8; ++i)
        state[4 + i] = ((uint32_t)key[4 * i] << 24) | ((uint32_t)key[4 * i + 1] << 16) | ((uint32_t)key[4 * i + 2] << 8) | ((uint32_t)key[4 * i + 3]);
    state[12] = counter;
    for (i = 0; i < 3; ++i)
        state[13 + i] = ((uint32_t)nonce[4 * i] << 24) | ((uint32_t)nonce[4 * i + 1] << 16) | ((uint32_t)nonce[4 * i + 2] << 8) | ((uint32_t)nonce[4 * i + 3]);

    while (len > 0) {
        chacha20_block(state, block);
        for (i = 0; i < 16; ++i) {
            keystream[4 * i] = block[i] >> 24;
            keystream[4 * i + 1] = block[i] >> 16;
            keystream[4 * i + 2] = block[i] >> 8;
            keystream[4 * i + 3] = block[i];
        }
        for (i = 0; i < 64 && i < len; ++i)
            ciphertext[i] = plaintext[i] ^ keystream[i];
        len -= i;
        plaintext += i;
        ciphertext += i;
        state[12]++;
    }
}
