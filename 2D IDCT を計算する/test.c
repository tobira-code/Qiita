#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void IDCT(int16_t F[][8], int16_t f[][8])
{
  /*
  ��k = cos(��*k/16)
  k	��(k)
  1	0.980785280403230
  2	0.923879532511287
  3	0.831469612302545
  4	0.707106781186548
  5	0.555570233019602
  6	0.382683432365090
  7	0.195090322016128
  */
  static const double g4 = 0.707106781186548;
  static const double g[4][7] = {
    /* row 0, 4 */
    {
      0.1733799806652680, /* g1 * 0.25 * g4 */
      0.1633203706095470, /* g2 * 0.25 * g4 */
      0.1469844503024200, /* g3 * 0.25 * g4 */
      0.1250000000000000, /* g4 * 0.25 * g4 */
      0.0982118697983878, /* g5 * 0.25 * g4 */
      0.0676495125182746, /* g6 * 0.25 * g4 */
      0.0344874224103679, /* g7 * 0.25 * g4 */
    },
    /* row 1, 7 */
    {
      0.2404849415639110, /* g1 * 0.25 * g1 */
      0.2265318615882220, /* g2 * 0.25 * g1 */
      0.2038732892122290, /* g3 * 0.25 * g1 */
      0.1733799806652680, /* g4 * 0.25 * g1 */
      0.1362237766939550, /* g5 * 0.25 * g1 */
      0.0938325693794663, /* g6 * 0.25 * g1 */
      0.0478354290456362, /* g7 * 0.25 * g1 */
    },
    /* row 2, 6 */
    {
      0.2265318615882220, /* g1 * 0.25 * g2 */
      0.2133883476483180, /* g2 * 0.25 * g2 */
      0.1920444391778540, /* g3 * 0.25 * g2 */
      0.1633203706095470, /* g4 * 0.25 * g2 */
      0.1283199917898340, /* g5 * 0.25 * g2 */
      0.0883883476483185, /* g6 * 0.25 * g2 */
      0.0450599888754343, /* g7 * 0.25 * g2 */
    },
    /* row 3, 5 */
    {
      0.2038732892122290, /* g1 * 0.25 * g3 */
      0.1920444391778540, /* g2 * 0.25 * g3 */
      0.1728354290456360, /* g3 * 0.25 * g3 */
      0.1469844503024200, /* g4 * 0.25 * g3 */
      0.1154849415639110, /* g5 * 0.25 * g3 */
      0.0795474112858021, /* g6 * 0.25 * g3 */
      0.0405529186026822, /* g7 * 0.25 * g3 */
    },
  };

  static const double t[3] = {
    0.414213562373095 /* t1 = g6/g2 */,
    0.198912367379658 /* t2 = g7/g1 */,
    0.668178637919299 /* t3 = g5/g3 */,
  };

  static const int row2idx[] = {
    0, 1, 2, 3, 0, 3, 2, 1,
  };

  static double _f[8][8];

  int i;

  // row
  // add 8*(3*8+8) = 256
  // mul 8*(6+4*4) = 176
  // C = A M P
  for (i=0; i<8; i++) {
    double P[8], M[8], tmp[6];
    int idx = row2idx[i];
    /* P */
    P[0]   = F[0][i];  /* 1 0 0 0 0 0 0 0 */
    P[1]   = F[2][i];  /* 0 0 1 0 0 0 0 0 */
    P[2]   = F[4][i];  /* 0 0 0 0 1 0 0 0 */
    P[3]   = F[6][i];  /* 0 0 0 0 0 0 1 0 */
    P[4]   = F[1][i];  /* 0 1 0 0 0 0 0 0 */
    P[5]   = F[3][i];  /* 0 0 0 1 0 0 0 0 */
    P[6]   = F[5][i];  /* 0 0 0 0 0 1 0 0 */
    P[7]   = F[7][i];  /* 0 0 0 0 0 0 0 1 */
    tmp[0] = P[0] * g[idx][3];
    tmp[1] = P[1] * g[idx][1];
    tmp[2] = P[1] * g[idx][5];
    tmp[3] = P[2] * g[idx][3];
    tmp[4] = P[3] * g[idx][5];
    tmp[5] = P[3] * g[idx][1];
    /* M */
    M[0]  = tmp[0] + tmp[1] + tmp[3] + tmp[4];                                         /*  g4  g2  g4  g6  0  0  0  0 */
    M[1]  = tmp[0] + tmp[2] - tmp[3] - tmp[5];                                         /*  g4  g6 -g4 -g2  0  0  0  0 */
    M[2]  = tmp[0] - tmp[2] - tmp[3] + tmp[5];                                         /*  g4 -g6 -g4  g2  0  0  0  0 */
    M[3]  = tmp[0] - tmp[1] + tmp[3] - tmp[4];                                         /*  g4 -g2  g4 -g6  0  0  0  0 */
    M[4]  = P[4] * g[idx][0] + P[5] * g[idx][2] + P[6] * g[idx][4] + P[7] * g[idx][6]; /*  0  0  0  0  g1  g3  g5  g7 */
    M[5]  = P[4] * g[idx][2] - P[5] * g[idx][6] - P[6] * g[idx][0] - P[7] * g[idx][4]; /*  0  0  0  0  g3 -g7 -g1 -g5 */
    M[6]  = P[4] * g[idx][4] - P[5] * g[idx][0] + P[6] * g[idx][6] + P[7] * g[idx][2]; /*  0  0  0  0  g5 -g1  g7  g3 */
    M[7]  = P[4] * g[idx][6] - P[5] * g[idx][4] + P[6] * g[idx][2] - P[7] * g[idx][0]; /*  0  0  0  0  g7 -g5  g3 -g1 */
    /* A */
    _f[0][i]  = M[0] + M[4];  /*  1  0  0  0  1  0  0  0 */
    _f[1][i]  = M[1] + M[5];  /*  0  1  0  0  0  1  0  0 */
    _f[2][i]  = M[2] + M[6];  /*  0  0  1  0  0  0  1  0 */
    _f[3][i]  = M[3] + M[7];  /*  0  0  0  1  0  0  0  1 */
    _f[4][i]  = M[3] - M[7];  /*  0  0  0  1  0  0  0 -1 */
    _f[5][i]  = M[2] - M[6];  /*  0  0  1  0  0  0 -1  0 */
    _f[6][i]  = M[1] - M[5];  /*  0  1  0  0  0 -1  0  0 */
    _f[7][i]  = M[0] - M[4];  /*  1  0  0  0 -1  0  0  0 */
  }

  // column
  // add 8*26 = 208
  // mul 8*8  = 64
  // C = A F E B D P
  for (i=0; i<8; i++) {
    double P[8], D[8], B[8], E[8], _F[8];
    /* P */
    P[0]   = _f[i][0];  /* 1 0 0 0 0 0 0 0 */
    P[1]   = _f[i][2];  /* 0 0 1 0 0 0 0 0 */
    P[2]   = _f[i][4];  /* 0 0 0 0 1 0 0 0 */
    P[3]   = _f[i][6];  /* 0 0 0 0 0 0 1 0 */
    P[4]   = _f[i][1];  /* 0 1 0 0 0 0 0 0 */
    P[5]   = _f[i][3];  /* 0 0 0 1 0 0 0 0 */
    P[6]   = _f[i][5];  /* 0 0 0 0 0 1 0 0 */
    P[7]   = _f[i][7];  /* 0 0 0 0 0 0 0 1 */
    
    /* D */
    /* g4  0  0  0  0  0  0  0 */
    /*  0  0 g4  0  0  0  0  0 */
    /*  0 g2  0  0  0  0  0  0 */
    /*  0  0  0 g2  0  0  0  0 */
    /*  0  0  0  0 g1  0  0  0 */
    /*  0  0  0  0  0  0  0 g1 */
    /*  0  0  0  0  0 g3  0  0 */
    /*  0  0  0  0  0  0 g3  0 */
    D[0] = P[0];
    D[1] = P[2];
    D[2] = P[1];
    D[3] = P[3];
    D[4] = P[4];
    D[5] = P[7];
    D[6] = P[5];
    D[7] = P[6];
    
    /* B t1=g6/g2, t2=g7/g1, t3=g5/g3 */
    /*  1  1  0  0  0  0  0  0 */
    /*  1 -1  0  0  0  0  0  0 */
    /*  0  0  1 t1  0  0  0  0 */
    /*  0  0 t1 -1  0  0  0  0 */
    /*  0  0  0  0  1 t2  0  0 */
    /*  0  0  0  0 t2 -1  0  0 */
    /*  0  0  0  0  0  0  1 t3 */
    /*  0  0  0  0  0  0 t3 -1 */
    B[0] =        D[0] +        D[1];
    B[1] =        D[0] -        D[1];
    B[2] =        D[2] + t[0] * D[3];
    B[3] = t[0] * D[2] -        D[3];
    B[4] =        D[4] + t[1] * D[5];
    B[5] = t[1] * D[4] -        D[5];
    B[6] =        D[6] + t[2] * D[7];
    B[7] = t[2] * D[6] -        D[7];
    
    /* E */
    E[0] = B[0] + B[2]; /* 1  0  1  0  0  0  0  0 */
    E[1] = B[1] + B[3]; /* 0  1  0  1  0  0  0  0 */
    E[2] = B[1] - B[3]; /* 0  1  0 -1  0  0  0  0 */
    E[3] = B[0] - B[2]; /* 1  0 -1  0  0  0  0  0 */
    E[4] = B[4] + B[6]; /* 0  0  0  0  1  0  1  0 */
    E[5] = B[4] - B[6]; /* 0  0  0  0  1  0 -1  0 */
    E[6] = B[5] + B[7]; /* 0  0  0  0  0  1  0  1 */
    E[7] = B[5] - B[7]; /* 0  0  0  0  0  1  0 -1 */
    
    /* F g=g4*/
    _F[0] = E[0];               /* 1  0  0  0  0  0  0  0 */
    _F[1] = E[1];               /* 0  1  0  0  0  0  0  0 */
    _F[2] = E[2];               /* 0  0  1  0  0  0  0  0 */
    _F[3] = E[3];               /* 0  0  0  1  0  0  0  0 */
    _F[4] = E[4];               /* 0  0  0  0  1  0  0  0 */
    _F[5] = g4 * (E[5] + E[6]); /* 0  0  0  0  0  g  g  0 */
    _F[6] = g4 * (E[5] - E[6]); /* 0  0  0  0  0  g -g  0 */
    _F[7] = E[7];               /* 0  0  0  0  0  0  0  1 */
    
    /* A */
    f[i][0]  = _F[0] + _F[4];    /* 1  0  0  0  1  0  0  0 */
    f[i][1]  = _F[1] + _F[5];    /* 0  1  0  0  0  1  0  0 */
    f[i][2]  = _F[2] + _F[6];    /* 0  0  1  0  0  0  1  0 */
    f[i][3]  = _F[3] + _F[7];    /* 0  0  0  1  0  0  0  1 */
    f[i][4]  = _F[3] - _F[7];    /* 0  0  0  1  0  0  0 -1 */
    f[i][5]  = _F[2] - _F[6];    /* 0  0  1  0  0  0 -1  0 */
    f[i][6]  = _F[1] - _F[5];    /* 0  1  0  0  0 -1  0  0 */
    f[i][7]  = _F[0] - _F[4];    /* 1  0  0  0 -1  0  0  0 */
  }
}

int main(int argc, char* argv[])
{
  int i,j;
  
  int16_t F[8][8] = {
    { 568,     0,     0,    -4,    -4,     0,     4,     0 },
    { -27,     9,    -4,    -4,     0,    -5,     5,    -5 },
    { -49,    -4,     4,     4,     0,     0,     0,     0 },
    { -12,    -4,     0,     0,     5,     0,     0,     0 },
    { -14,    -5,     0,     0,     0,     0,     0,     0 },
    {  -5,     0,     0,     0,     0,     0,     0,     0 },
    {  -5,     0,     0,     0,     0,     0,     0,     0 },
    {   0,     0,     0,     0,     0,     0,     0,     1 },
  };
  /* f[8][8] after IDCT
    52   53   54   53   53   56   50   56
    67   70   71   68   65   68   62   66
    74   78   80   74   71   74   70   71
    74   78   81   78   75   77   75   76
    75   76   80   78   78   77   78   79
    75   74   76   76   75   75   77   77
    73   70   72   70   72   72   77   73
    66   65   66   63   66   69   75   69
  */
  
  int16_t f[8][8];
  
  IDCT(F, f);
  
  for (i=0; i<8; i++) {
    for (j=0; j<8; j++) {
      printf("% 4d ", f[i][j]);
    }
    printf("\n");
  }
  printf("\n");
  
  return 0;
}
