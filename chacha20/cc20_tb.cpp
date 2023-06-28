#include "cc20.h" 

void cc20_algo_sw(uint8_t key[32], uint32_t counter, uint8_t nonce[12], uint8_t plaintext[], int len){

    //***************** Initializing State Matrix (Start) *****************//
    uint32_t matrix[16], block[16];
    uint32_t constant[4] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; //Constants given in the algorithm

    matrix[0] = constant[0]; matrix[1] = constant[1]; matrix[2] = constant[2]; matrix[3] = constant[3]; // Constant

    for (int i = 0; i < 8; i++){
        matrix[4+i] = ((uint32_t)key[4*i + 3] << 24) | ((uint32_t)key[4*i + 2] << 16) | ((uint32_t)key[4*i + 1] << 8) | ((uint32_t)key[4*i]);
    }

    matrix[12] =  counter; // Counter

    for (int i = 0; i < 3; i++){
        matrix[13+i] = ((uint32_t)nonce[4*i + 3] << 24) | ((uint32_t)nonce[4*i + 2] << 16) | ((uint32_t)nonce[4*i + 1] << 8) | ((uint32_t)nonce[4*i]);
    }

    printf("\nInitial State Matrix:\n");
    for (int i = 0; i < 16; i++) {
        printf("%08x ", matrix[i]); // Initial State Matrix
        if (i%4 == 3) printf("\n");
    }
    //***************** Initializing State Matrix (End) *****************//

    //***************** Generating Cipher (Start) *****************//
    uint8_t cipher[len];
    uint8_t keystream[len];
    int num_blocks = len/64;
    int rem = len%64;

    for (int i = 0; i < num_blocks; i++){
		chacha20_block(matrix, block);
		printf("\nBlock %d:\n", i+1);
		for (int j = 0; j < 16; j++) {
			printf("%08x ", block[j]); // ChaCha20 Block
			if (j%4 == 3) printf("\n");
		}

		for (int j = 0; j < 16; ++j) {
            keystream[i*64 + 4*j + 3] = rightShift(block[j], 24);
            keystream[i*64 + 4*j + 2] = rightShift(block[j], 16);
            keystream[i*64 + 4*j + 1] = rightShift(block[j], 8);
            keystream[i*64 + 4*j] = block[j];
        }
        matrix[12]++;
    }

    if (rem != 0){
        chacha20_block(matrix, block);
		printf("\nBlock %d:\n", num_blocks+1);
		for (int j = 0; j < 16; j++) {
			printf("%08x ", block[j]); // ChaCha20 Block
			if (j%4 == 3) printf("\n");
		}

        for (int j = 0; j < 16; ++j) {
            keystream[num_blocks*64 + 4*j + 3] = rightShift(block[j], 24);
            keystream[num_blocks*64 + 4*j + 2] = rightShift(block[j], 16);
            keystream[num_blocks*64 + 4*j + 1] = rightShift(block[j], 8);
            keystream[num_blocks*64 + 4*j] = block[j];
        }
    }

	for (int i = 0; i < len; i++){
		cipher[i] = plaintext[i] ^ keystream[i];
	}
    //***************** Generating Cipher (End) *****************//

	printf("\nKeystream:\n");
	for (int i = 0; i < len; i++){
		printf("%02x ", keystream[i]);
		if (i%16 == 15) printf("\n");
	}
	printf("\n");
    printf("\nCipher:\n");
    for (int i = 0; i < len; i++){
        printf("%02x ", cipher[i]);
		if (i%16 == 15) printf("\n");
    }
	printf("\n");
}


int main(){
    // hls::stream<axis_data> input, output;
    // axis_data local_stream;

    // ***************** Test Vector 1 (Start) ***************** //
	uint8_t key1[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f};
	uint8_t counter1[4] = {0x00, 0x00, 0x00, 0x01};
	uint8_t nonce1[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00};
	char plaintext1_[] = "Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it.";
	int len1 = sizeof(plaintext1_)-1; // -1 to remove the null character
	uint8_t plaintext1[len1];
	textToHexArray(plaintext1_, plaintext1, len1);
	// ***************** Test Vector 1 (End) ***************** //

	// ***************** Test Vector 2 (Start) ***************** //
    uint8_t key2[32] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    uint32_t counter2 = 1;
    uint8_t nonce2[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    char plaintext2_[] = "Any submission to the IETF intended by the Contributor for publication as all or part of an IETF Internet-Draft or RFC and any statement made within the context of an IETF activity is considered an \"IETF Contribution\". Such statements include oral statements in IETF sessions, as well as written and electronic communications made at any time or place, which are addressed to";
    int len2 = sizeof(plaintext2_)-1; // -1 to remove the null character
    uint8_t plaintext2[len2];
    textToHexArray(plaintext2_, plaintext2, len2);
	// ***************** Test Vector 2 (End) ***************** //

	// ***************** Test Vector 3 (Start) ***************** //
    uint8_t key3[32] = {0x1c, 0x92, 0x40, 0xa5, 0xeb, 0x55, 0xd3, 0x8a, 0xf3, 0x33, 0x88, 0x86, 0x04, 0xf6, 0xb5, 0xf0, 0x47, 0x39, 0x17, 0xc1, 0x40, 0x2b, 0x80, 0x09, 0x9d, 0xca, 0x5c, 0xbc, 0x20, 0x70, 0x75, 0xc0};
	uint32_t counter3 = 42;
	uint8_t nonce3[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
    char plaintext3_[] = "'Twas brillig, and the slithy toves did gyre and gimble in the wabe; All mimsy were the borogoves, and the mome raths outgrabe.";
    int len3 = sizeof(plaintext3_)-1; // -1 to remove the null character
    uint8_t plaintext3[len3];
    textToHexArray(plaintext3_, plaintext3, len3);
	// ***************** Test Vector 3 (End) ***************** //

	cc20_algo_sw(key3, counter3, nonce3, plaintext3, len3);
}
