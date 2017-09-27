package com.example.issi.testandroid;

/**
 * Created by ISSI on 2017-08-21.
 */

public class AES {


    byte[][] S_BOX = {
            //		 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
            {0x63, 0x7c, 0x77, 0x7b, (byte) 0xf2, 0x6b, 0x6f, (byte) 0xc5, 0x30, 0x01, 0x67, 0x2b, (byte) 0xfe, (byte) 0xd7, (byte) 0xab, 0x76},    // 0
            {(byte) 0xca, (byte) 0x82, (byte) 0xc9, 0x7d, (byte) 0xfa, 0x59, 0x47, (byte) 0xf0, (byte) 0xad, (byte) 0xd4, (byte) 0xa2, (byte) 0xaf, (byte) 0x9c, (byte) 0xa4, 0x72, (byte) 0xc0},    // 1
            {(byte) 0xb7, (byte) 0xfd, (byte) 0x93, 0x26, 0x36, 0x3f, (byte) 0xf7, (byte) 0xcc, 0x34, (byte) 0xa5, (byte) 0xe5, (byte) 0xf1, 0x71, (byte) 0xd8, 0x31, 0x15},    // 2
            {0x04, (byte) 0xc7, 0x23, (byte) 0xc3, 0x18, (byte) 0x96, 0x05, (byte) 0x9a, 0x07, 0x12, (byte) 0x80, (byte) 0xe2, (byte) 0xeb, 0x27, (byte) 0xb2, 0x75},    // 3
            {0x09, (byte) 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, (byte) 0xa0, 0x52, 0x3b, (byte) 0xd6, (byte) 0xb3, 0x29, (byte) 0xe3, 0x2f, (byte) 0x84},    // 4
            {0x53, (byte) 0xd1, 0x00, (byte) 0xed, 0x20, (byte) 0xfc, (byte) 0xb1, 0x5b, 0x6a, (byte) 0xcb, (byte) 0xbe, 0x39, 0x4a, 0x4c, 0x58, (byte) 0xcf},    // 5
            {(byte) 0xd0, (byte) 0xef, (byte) 0xaa, (byte) 0xfb, 0x43, 0x4d, 0x33, (byte) 0x85, 0x45, (byte) 0xf9, 0x02, 0x7f, 0x50, 0x3c, (byte) 0x9f, (byte) 0xa8},    // 6
            {0x51, (byte) 0xa3, 0x40, (byte) 0x8f, (byte) 0x92, (byte) 0x9d, 0x38, (byte) 0xf5, (byte) 0xbc, (byte) 0xb6, (byte) 0xda, 0x21, 0x10, (byte) 0xff, (byte) 0xf3, (byte) 0xd2},    // 7
            {(byte) 0xcd, 0x0c, 0x13, (byte) 0xec, 0x5f, (byte) 0x97, 0x44, 0x17, (byte) 0xc4, (byte) 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},    // 8
            {0x60, (byte) 0x81, 0x4f, (byte) 0xdc, 0x22, 0x2a, (byte) 0x90, (byte) 0x88, 0x46, (byte) 0xee, (byte) 0xb8, 0x14, (byte) 0xde, 0x5e, 0x0b, (byte) 0xdb},    // 9
            {(byte) 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, (byte) 0xc2, (byte) 0xd3, (byte) 0xac, 0x62, (byte) 0x91, (byte) 0x95, (byte) 0xe4, 0x79},    // a
            {(byte) 0xe7, (byte) 0xc8, 0x37, 0x6d, (byte) 0x8d, (byte) 0xd5, 0x4e, (byte) 0xa9, 0x6c, 0x56, (byte) 0xf4, (byte) 0xea, 0x65, 0x7a, (byte) 0xae, 0x08},    // b
            {(byte) 0xba, 0x78, 0x25, 0x2e, 0x1c, (byte) 0xa6, (byte) 0xb4, (byte) 0xc6, (byte) 0xe8, (byte) 0xdd, 0x74, 0x1f, 0x4b, (byte) 0xbd, (byte) 0x8b, (byte) 0x8a},    // c
            {0x70, 0x3e, (byte) 0xb5, 0x66, 0x48, 0x03, (byte) 0xf6, 0x0e, 0x61, 0x35, 0x57, (byte) 0xb9, (byte) 0x86, (byte) 0xc1, 0x1d, (byte) 0x9e},    // d
            {(byte) 0xe1, (byte) 0xf8, (byte) 0x98, 0x11, 0x69, (byte) 0xd9, (byte) 0x8e, (byte) 0x94, (byte) 0x9b, 0x1e, (byte) 0x87, (byte) 0xe9, (byte) 0xce, 0x55, 0x28, (byte) 0xdf},    // e
            {(byte) 0x8c, (byte) 0xa1, (byte) 0x89, 0x0d, (byte) 0xbf, (byte) 0xe6, 0x42, 0x68, 0x41, (byte) 0x99, 0x2d, 0x0f, (byte) 0xb0, 0x54, (byte) 0xbb, 0x16}    // f
    };

    byte[][] Inv_S_BOX = {
            //0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
            {0x52, 0x09, 0x6a, (byte) 0xd5, 0x30, 0x36, (byte) 0xa5, 0x38, (byte) 0xbf, 0x40, (byte) 0xa3, (byte) 0x9e, (byte) 0x81, (byte) 0xf3, (byte) 0xd7, (byte) 0xfb},    // 0
            {0x7c, (byte) 0xe3, 0x39, (byte) 0x82, (byte) 0x9b, 0x2f, (byte) 0xff, (byte) 0x87, 0x34, (byte) 0x8e, 0x43, 0x44, (byte) 0xc4, (byte) 0xde, (byte) 0xe9, (byte) 0xcb},    // 1
            {0x54, 0x7b, (byte) 0x94, 0x32, (byte) 0xa6, (byte) 0xc2, 0x23, 0x3d, (byte) 0xee, 0x4c, (byte) 0x95, 0x0b, 0x42, (byte) 0xfa, (byte) 0xc3, 0x4e},    // 2
            {0x08, 0x2e, (byte) 0xa1, 0x66, 0x28, (byte) 0xd9, 0x24, (byte) 0xb2, 0x76, 0x5b, (byte) 0xa2, 0x49, 0x6d, (byte) 0x8b, (byte) 0xd1, 0x25},    // 3
            {0x72, (byte) 0xf8, (byte) 0xf6, 0x64, (byte) 0x86, 0x68, (byte) 0x98, 0x16, (byte) 0xd4, (byte) 0xa4, 0x5c, (byte) 0xcc, 0x5d, 0x65, (byte) 0xb6, (byte) 0x92},    // 4
            {0x6c, 0x70, 0x48, 0x50, (byte) 0xfd, (byte) 0xed, (byte) 0xb9, (byte) 0xda, 0x5e, 0x15, 0x46, 0x57, (byte) 0xa7, (byte) 0x8d, (byte) 0x9d, (byte) 0x84},    // 5
            {(byte) 0x90, (byte) 0xd8, (byte) 0xab, 0x00, (byte) 0x8c, (byte) 0xbc, (byte) 0xd3, 0x0a, (byte) 0xf7, (byte) 0xe4, 0x58, 0x05, (byte) 0xb8, (byte) 0xb3, 0x45, 0x06},    // 6
            {(byte) 0xd0, 0x2c, 0x1e, (byte) 0x8f, (byte) 0xca, 0x3f, 0x0f, 0x02, (byte) 0xc1, (byte) 0xaf, (byte) 0xbd, 0x03, 0x01, 0x13, (byte) 0x8a, 0x6b},    // 7
            {0x3a, (byte) 0x91, 0x11, 0x41, 0x4f, 0x67, (byte) 0xdc, (byte) 0xea, (byte) 0x97, (byte) 0xf2, (byte) 0xcf, (byte) 0xce, (byte) 0xf0, (byte) 0xb4, (byte) 0xe6, 0x73},    // 8
            {(byte) 0x96, (byte) 0xac, 0x74, 0x22, (byte) 0xe7, (byte) 0xad, 0x35, (byte) 0x85, (byte) 0xe2, (byte) 0xf9, 0x37, (byte) 0xe8, 0x1c, 0x75, (byte) 0xdf, 0x6e},    // 9
            {0x47, (byte) 0xf1, 0x1a, 0x71, 0x1d, 0x29, (byte) 0xc5, (byte) 0x89, 0x6f, (byte) 0xb7, 0x62, 0x0e, (byte) 0xaa, 0x18, (byte) 0xbe, 0x1b},    // a
            {(byte) 0xfc, 0x56, 0x3e, 0x4b, (byte) 0xc6, (byte) 0xd2, 0x79, 0x20, (byte) 0x9a, (byte) 0xdb, (byte) 0xc0, (byte) 0xfe, 0x78, (byte) 0xcd, 0x5a, (byte) 0xf4},    // b
            {0x1f, (byte) 0xdd, (byte) 0xa8, 0x33, (byte) 0x88, 0x07, (byte) 0xc7, 0x31, (byte) 0xb1, 0x12, 0x10, 0x59, 0x27, (byte) 0x80, (byte) 0xec, 0x5f},    // c
            {0x60, 0x51, 0x7f, (byte) 0xa9, 0x19, (byte) 0xb5, 0x4a, 0x0d, 0x2d, (byte) 0xe5, 0x7a, (byte) 0x9f, (byte) 0x93, (byte) 0xc9, (byte) 0x9c, (byte) 0xef},    // d
            {(byte) 0xa0, (byte) 0xe0, 0x3b, 0x4d, (byte) 0xae, 0x2a, (byte) 0xf5, (byte) 0xb0, (byte) 0xc8, (byte) 0xeb, (byte) 0xbb, 0x3c, (byte) 0x83, 0x53, (byte) 0x99, 0x61},    // e
            {0x17, 0x2b, 0x04, 0x7e, (byte) 0xba, 0x77, (byte) 0xd6, 0x26, (byte) 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d}    // f
    };

    public void Inv_KeySchedule(byte[][] CipherKey, byte[][] S_BOX, int i) {
        byte[][] Rcon = {
                {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, (byte) 0x80, 0x1b, 0x36},
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
                {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
        };

        byte[][] tmp = {
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
        };
        byte[] tmp2 = {0, 0, 0, 0};

        int j, k, x, y;

        for (j = 0; j < 3; j++) {// RotWord
            tmp2[j] = CipherKey[j + 1][3];
        }
        tmp2[3] = CipherKey[0][3];

        for (j = 0; j < 4; j++) {// SubBytes
            x = (tmp2[j] >> 4) & (0x0F);
            y = (tmp2[j] - (x << 4)) & (0x0F);
            tmp2[j] = S_BOX[x][y];
        }

        for (j = 0; j < 4; j++) {
            tmp[j][0] = (byte) (CipherKey[j][0] ^ tmp2[j] ^ Rcon[j][i]);
        }


        for (j = 0; j < 4; j++) {
            for (k = 1; k < 4; k++) {
                tmp[j][k] = (byte) (CipherKey[j][k] ^ tmp[j][k - 1]);
            }
        }

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                CipherKey[i][j] = tmp[i][j];
            }
        }

    }

    public void Inv_AddRoundKey(byte[][] state, byte[][] RoundKey) {
        int i, j;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                state[i][j] ^= RoundKey[i][j];
            }
        }

    }
/*
    public void Inv_initCipher(byte[][] CipherKey) {
        byte[][] tmp = { // Original Cipher Key
                {0x2b, 0x28, (byte) 0xab, 0x09},
                {0x7e, (byte) 0xae, (byte) 0xf7, (byte) 0xcf},
                {0x15, (byte) 0xd2, 0x15, 0x4f},
                {0x16, (byte) 0xa6, (byte) 0x88, 0x3c}
        };
        int i, j;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                CipherKey[i][j] = tmp[i][j];
            }
        }

    }*/

    public byte[][] init_CipherKey = new byte[4][4];

    public void Inv_ShiftRows(byte[][] state) {
        int i, j, k;
        byte tmp;

        for (i = 3; i > 0; i--) {
            for (j = i - 1; j < 3; j++) {
                tmp = state[i][0];
                for (k = 0; k < 3; k++) {
                    state[i][k] = state[i][k + 1];
                }
                state[i][3] = tmp;
            }
        }
    }

    public void Inv_SubBytes(byte[][] state, byte[][] S_BOX) {
        int i, j, x, y;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                x = (state[i][j] >> 4) & (0x0F);
                y = (state[i][j] - (x << 4)) & (0x0F);
                state[i][j] = S_BOX[x][y];
            }
        }
    }

    public void Inv_MixColumns(byte[][] state) {
        byte[] a = {0, 0, 0, 0};
        int i, j;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                a[j] = state[j][i];
            }
            state[0][i] = (byte) (0xFF & (Multiply(a[0], (byte) 0x0e) ^ Multiply(a[1], (byte) 0x0b) ^ Multiply(a[2], (byte) 0x0d) ^ Multiply(a[3], (byte) 0x09)));
            state[1][i] = (byte) (0xFF & (Multiply(a[0], (byte) 0x09) ^ Multiply(a[1], (byte) 0x0e) ^ Multiply(a[2], (byte) 0x0b) ^ Multiply(a[3], (byte) 0x0d)));
            state[2][i] = (byte) (0xFF & (Multiply(a[0], (byte) 0x0d) ^ Multiply(a[1], (byte) 0x09) ^ Multiply(a[2], (byte) 0x0e) ^ Multiply(a[3], (byte) 0x0b)));
            state[3][i] = (byte) (0xFF & (Multiply(a[0], (byte) 0x0b) ^ Multiply(a[1], (byte) 0x0d) ^ Multiply(a[2], (byte) 0x09) ^ Multiply(a[3], (byte) 0x0e)));
        }
    }

    public byte Multiply(byte x, byte y) {
        return (byte) (((y & 1) * x) ^ ((((y >> 1) & (0x7F)) & 1) * xtime(x)) ^ ((((y >> 2) & (0x3F)) & 1) * xtime(xtime(x))) ^ ((((y >> 3) & (0x1F)) & 1) * xtime(xtime(xtime(x)))) ^ ((((y >> 4) & (0x0F)) & 1) * xtime(xtime(xtime(xtime(x))))));
    }

    public byte xtime(byte x) {
        return (byte) ((x << 1) ^ ((((x >> 7) & (0x01)) & 1) * 0x1b));
    }


    public byte[][] Decrypt(byte[][] CipherKey, byte[][] state) {

        int i, j = 0;

        for(i = 0; i < 4; i++)
            for(j = 0; j < 4; j++)
                init_CipherKey[i][j] = CipherKey[i][j];

        for (i = 0; i < 10; i++)
            Inv_KeySchedule(CipherKey, S_BOX, i);
        Inv_AddRoundKey(state, CipherKey);

        for(i = 0; i < 4; i++)
            for(j = 0; j < 4; j++)
                CipherKey[i][j] = init_CipherKey[i][j];

        Inv_ShiftRows(state);
        Inv_SubBytes(state, Inv_S_BOX);

        for (i = 9; i > 0; i--) {
            //매 Round의 역산마다 해당 CipherKey 적용하여 역산한 후 초기화
            for (j = 0; j < i; j++) {
                Inv_KeySchedule(CipherKey, S_BOX, j);
            }
            Inv_AddRoundKey(state, CipherKey);

            for(int x = 0; x < 4; x++)
                for(int y = 0; y < 4; y++)
                    CipherKey[x][y] = init_CipherKey[x][y];

            Inv_MixColumns(state);
            Inv_ShiftRows(state);
            Inv_SubBytes(state, Inv_S_BOX);
        }

        Inv_AddRoundKey(state, CipherKey);

       /*
        int k = 0;
        int flag = 0;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (state[i][j] == 0) {
                    flag = 1;
                    break;
                } else {
                    flag = 0;
                    temp += (char) state[i][j];
                }
            }
            if (flag == 1)
                break;
        }*/

        //temp = temp_state.toString();


        return state;

    }


    public void AddRoundKey(byte[][] state, byte[][] RoundKey) {
        int i, j;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                state[i][j] ^= RoundKey[i][j];
            }
        }
    }

    public void SubBytes(byte[][] state, byte[][] S_BOX) {
        int i, j, x, y;

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                x = (state[i][j] >> 4) & (0x0F);
                y = (state[i][j] - (x << 4)) & (0x0F);
                state[i][j] = S_BOX[x][y];
            }
        }
    }

    public void ShiftRows(byte[][] state)
    {
        int i, j, k;
        byte tmp;

        for (i = 0; i<4; i++){
            for (j = 4 - i; j<4; j++){
                tmp = state[i][0];
                for (k = 0; k<4 - 1; k++){
                    state[i][k] = state[i][k + 1];
                }
                state[i][3] = tmp;
            }
        }

    }

    public void MixColumns(byte[][] state){

        byte[] a = {0 ,0,0,0};
        byte[] b = {0,0,0,0};
        byte h=0;

        int i, j;

        for (i = 0; i<4; i++){
            for (j = 0; j<4; j++){
                a[j] = state[j][i];
                h = (byte)((state[j][i] >> 7) );
                b[j] = (byte)(state[j][i] << 1);
                b[j] ^= 0x1b & h;
            }
            state[0][i] = (byte)(b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]);
            state[1][i] = (byte)(b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]);
            state[2][i] = (byte)(b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]);
            state[3][i] = (byte)(b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]);
        }

    }

    public void KeySchedule(byte[][] CipherKey, byte[][] S_BOX, int i)
    {
        byte[][] Rcon = {
                { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, (byte)0x80, 0x1b, 0x36 },
                { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
                { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
        };
        byte[][] tmp = {
                {0,0,0,0},
                {0,0,0,0},
                {0,0,0,0},
                {0,0,0,0}
        };
        byte[] tmp2 = {0,0,0,0};
        int j, k, x, y;

        for (j = 0; j<3; j++){// RotWord
            tmp2[j] = CipherKey[j + 1][3];
        }
        tmp2[3] = CipherKey[0][3];

        for (j = 0; j<4; j++){// SubBytes
            x = (tmp2[j] >> 4) & (0x0F);
            y = (tmp2[j] - (x << 4)) & (0x0F);
            tmp2[j] = S_BOX[x][y];
        }
        // Each round's Wi_1 value
        for (j = 0; j<4; j++){
            tmp[j][0] = (byte)(CipherKey[j][0] ^ tmp2[j] ^ Rcon[j][i]);
        }
        // Wi_2 ~ Wi_4
        for (j = 0; j<4; j++){
            for (k = 1; k<4; k++){
                tmp[j][k] = (byte)(CipherKey[j][k] ^ tmp[j][k - 1]);
            }
        }
        for (i = 0; i<4; i++){
            for (j = 0; j<4; j++){
                CipherKey[i][j] = tmp[i][j];
            }
        }


    }

    public byte[][] Encrypt(byte[][] state)
    {
        String temp;

        temp="";
        byte[][] CipherKey = {			// Cipher Key to Key Schedule
                { 0x2b, 0x28, (byte)0xab, 0x09 },
                { 0x7e, (byte)0xae, (byte)0xf7, (byte)0xcf },
                { 0x15, (byte)0xd2, 0x15, 0x4f },
                { 0x16, (byte)0xa6, (byte)0x88, 0x3c }
        };

        int i, j;


        AddRoundKey(state, CipherKey); //ok

        for (i = 0; i<9; i++){
            SubBytes(state, S_BOX); //ok
            ShiftRows(state);//ok
            MixColumns(state);//ok
            KeySchedule(CipherKey, S_BOX, i);
            AddRoundKey(state, CipherKey);
        }

        SubBytes(state, S_BOX);
        ShiftRows(state);
        KeySchedule(CipherKey, S_BOX, i);

        AddRoundKey(state, CipherKey);


        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                temp += (char)state[i][j];


        return state;
    }


}

