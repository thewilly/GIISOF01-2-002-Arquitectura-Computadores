 97 for (int i = 0; i < IMAGES_SIZE; i += PIXELS_PER_DATA_PACKAGE) {
movl    $0x0,-0x214(%rbp)
mov     -0x214(%rbp),%eax
cmp     -0x204(%rbp),%eax
jge     0x555555557cbb <main()+1201>
 100 dataPackageImage1 = _mm_loadu_ps(&p_compImage1[i]);
mov     -0x214(%rbp),%eax
cltq    
lea     0x0(,%rax,4),%rdx
mov     -0x1f8(%rbp),%rax
add     %rdx,%rax
mov     %rax,-0x1d0(%rbp)
 934 return *(__m128_u *)__P;
mov     -0x1d0(%rbp),%rax
vmovups (%rax),%xmm0
 100 dataPackageImage1 = _mm_loadu_ps(&p_compImage1[i]);
vmovaps %xmm0,-0x190(%rbp)
 101 dataPackageImage2 = _mm_loadu_ps(&p_compImage2[i]);
mov     -0x214(%rbp),%eax
cltq    
lea     0x0(,%rax,4),%rdx
mov     -0x1f0(%rbp),%rax
add     %rdx,%rax
mov     %rax,-0x1d8(%rbp)
mov     -0x1d8(%rbp),%rax
vmovups (%rax),%xmm0
vmovaps %xmm0,-0x180(%rbp)
vmovaps -0x190(%rbp),%xmm0
vmovaps %xmm0,-0xd0(%rbp)
vmovaps -0x190(%rbp),%xmm0
vmovaps %xmm0,-0xc0(%rbp)
 198 return (__m128) ((__v4sf)__A * (__v4sf)__B);
vmovaps -0xd0(%rbp),%xmm0
vmulps  -0xc0(%rbp),%xmm0,%xmm0
 104 squareDP1 = _mm_mul_ps(dataPackageImage1, dataPackageImage1);
vmovaps %xmm0,-0x170(%rbp)
vmovaps -0x180(%rbp),%xmm0
00vmovaps %xmm0,-0xf0(%rbp)
vmovaps -0x180(%rbp),%xmm0
vmovaps %xmm0,-0xe0(%rbp)
vmovaps -0xf0(%rbp),%xmm0
vmulps  -0xe0(%rbp),%xmm0,%xmm0
 105 squareDP2 = _mm_mul_ps(dataPackageImage2, dataPackageImage2);
vmovaps %xmm0,-0x160(%rbp)
vmovaps -0x170(%rbp),%xmm0
vmovaps %xmm0,-0x110(%rbp)
vmovaps -0x160(%rbp),%xmm0
vmovaps %xmm0,-0x100(%rbp)
 186 return (__m128) ((__v4sf)__A + (__v4sf)__B);
vmovaps -0x110(%rbp),%xmm0
vaddps  -0x100(%rbp),%xmm0,%xmm0
 108 productSquareDP1SquareDP2 = _mm_add_ps(squareDP1, squareDP2);
vmovaps %xmm0,-0x150(%rbp)
vmovaps -0x150(%rbp),%xmm0
vmovaps %xmm0,-0x120(%rbp)
 210 return (__m128) __builtin_ia32_sqrtps ((__v4sf)__A);
vsqrtps -0x120(%rbp),%xmm0
 111 squarerootDPS = _mm_sqrt_ps(productSquareDP1SquareDP2);
vmovaps %xmm0,-0x140(%rbp)
vmovaps -0x140(%rbp),%xmm0
vmovaps %xmm0,-0xa0(%rbp)
vmovaps -0x1a0(%rbp),%xmm0
vmovaps %xmm0,-0x90(%rbp)
 204 return (__m128) ((__v4sf)__A / (__v4sf)__B);
vmovaps -0xa0(%rbp),%xmm0
vdivps  -0x90(%rbp),%xmm0,%xmm0
 114 division = _mm_div_ps(squarerootDPS, V_SQRT2);
vmovaps %xmm0,-0x130(%rbp)
 117 _mm_storeu_ps(&p_dstImage[i], division);
mov     -0x214(%rbp),%eax
cltq    
lea     0x0(,%rax,4),%rdx
mov     -0x1e8(%rbp),%rax
add     %rdx,%rax
mov     %rax,-0x1c8(%rbp)
vmovaps -0x130(%rbp),%xmm0
vmovaps %xmm0,-0xb0(%rbp)
 983 *(__m128_u *)__P = __A;
vmovaps -0xb0(%rbp),%xmm0
mov     -0x1c8(%rbp),%rax
vmovups %xmm0,(%rax)
