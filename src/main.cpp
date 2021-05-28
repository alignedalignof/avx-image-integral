#include <vector>
#include <cmath>
#include <ctime>
#include <cstdarg>
#include <chrono>
#include <cstdio>

using namespace std;
using namespace std::chrono;

static clock_t start = clock();
static void msprintf(const char* fmt, ...)
{
	long ms = 1000*(clock() - start);
	ms /= CLOCKS_PER_SEC ;
	printf("[%li ms] ", ms);
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\r\n");
}

__attribute__((optimize("O0"))) static void avxii(const uint8_t* img, uint32_t* integral, int w, int h)
{
	__asm__ __volatile__ (
	".macro acc32x;"
		"vpcmpeqq ymm3, ymm3, ymm3;"
		"vpsrld ymm3, ymm3, 16;"
		"vpand ymm4, ymm2, ymm3;"
		"vpand ymm3, ymm1, ymm3;"
		"vpslld ymm3, ymm3, 16;"
		"vpslld ymm4, ymm4, 16;"
		"vpaddw ymm1, ymm1, ymm3;"
		"vpaddw ymm2, ymm2, ymm4;"
		"vpcmpeqq ymm3, ymm3, ymm3;"
		"vpslld ymm3, ymm3, 16;"
		"vpand ymm4, ymm2, ymm3;"
		"vpand ymm3, ymm1, ymm3;"
		"vpshuflw ymm3, ymm3, 80;"
		"vpshuflw ymm4, ymm4, 80;"
		"vpshufhw ymm3, ymm3, 80;"
		"vpshufhw ymm4, ymm4, 80;"
		"vpaddw ymm1, ymm1, ymm3;"
		"vpaddw ymm2, ymm2, ymm4;"
		"vpcmpeqq ymm3, ymm3, ymm3;"
		"vpsllq ymm3, ymm3, 48;"
		"vpand ymm4, ymm2, ymm3;"
		"vpand ymm3, ymm1, ymm3;"
		"vpslldq ymm3, ymm3, 2;"
		"vpslldq ymm4, ymm4, 2;"
		"vpshufhw ymm3, ymm3, 0;"
		"vpshufhw ymm4, ymm4, 0;"
		"vpaddw ymm1, ymm1, ymm3;"
		"vpaddw ymm2, ymm2, ymm4;"
		"vpcmpeqq ymm3, ymm3, ymm3;"
		"vpslldq ymm3, ymm3, 14;"
		"vpand ymm4, ymm2, ymm3;"
		"vpand ymm3, ymm1, ymm3;"
		"vpermq ymm3, ymm3, 80;"
		"vpermq ymm4, ymm4, 80;"
		"vpshuflw ymm3, ymm3, 255;"
		"vpshuflw ymm4, ymm4, 255;"
		"vpshufhw ymm3, ymm3, 255;"
		"vpshufhw ymm4, ymm4, 255;"
		"vpaddw ymm1, ymm1, ymm3;"
		"vpaddw ymm2, ymm2, ymm4;"
		"vpermq ymm3, ymm1, 255;"
		"vpsrlq ymm3, ymm3, 48;"
		"vpbroadcastw ymm3, xmm3;"
		"vpaddw ymm2, ymm2, ymm3;"
	".endm;"
		"vzeroall;"
		"xor %[ofs], %[ofs];"
		"vmovdqu ymm0, [%[img]];"
	"firstx:"
		"vpmovzxbw ymm1, xmm0;"
		"vpermq ymm0, ymm0, 14;"
		"vpmovzxbw ymm2, xmm0;"
		"vmovdqu ymm0, [%[img] + %[ofs] + 32];"
		"acc32x;"
		"vpmovzxwd ymm3, xmm1;"
		"vpmovzxwd ymm4, xmm2;"
		"vpaddd ymm3, ymm7, ymm3;"
		"vpaddd ymm4, ymm7, ymm4;"
		"vmovdqu [%[integral]], ymm3;"
		"vmovdqu [%[integral] + 64], ymm4;"
		"vpermq ymm1, ymm1, 14;"
		"vpermq ymm2, ymm2, 14;"
		"vpmovzxwd ymm1, xmm1;"
		"vpmovzxwd ymm2, xmm2;"
		"vpaddd ymm1, ymm7, ymm1;"
		"vpaddd ymm2, ymm7, ymm2;"
		"vmovdqu [%[integral] + 32], ymm1;"
		"vmovdqu [%[integral] + 96], ymm2;"
		"vpermq ymm7, ymm2, 0xff;"
		"vpsrlq ymm7, ymm7, 32;"
		"vpbroadcastd ymm7, xmm7;"
		"add %[ofs], 32;"
		"add %[integral], 128;"
		"cmp %[ofs], %[w];"
		"jl firstx;"
		"dec %[h];"
		"cmp %[h], 0;"
		"je endx;"
		"sub %[integral], %[w];"
		"sub %[integral], %[w];"
		"sub %[integral], %[w];"
		"sub %[integral], %[w];"
	"rowx:"
		"xor %[ofs], %[ofs];"
		"add %[img], %[w];"
		"vpxor ymm7, ymm7, ymm7;"
	"blockx:"
		"vpmovzxbw ymm1, xmm0;"
		"vpermq ymm0, ymm0, 14;"
		"vpmovzxbw ymm2, xmm0;"
		"vmovdqu ymm0, [%[img] + %[ofs] + 32];"
		"vmovdqu ymm5, [%[integral]];"
		"vmovdqu ymm6, [%[integral] + 32];"
		"acc32x;"
		"vpmovzxwd ymm4, xmm1;"
		"vpaddd ymm4, ymm5, ymm4;"
		"vmovdqu ymm5, [%[integral] + 64];"
		"vpermq ymm1, ymm1, 14;"
		"vpmovzxwd ymm1, xmm1;"
		"vpaddd ymm1, ymm6, ymm1;"
		"vmovdqu ymm6, [%[integral] + 96];"
		"vpaddd ymm4, ymm7, ymm4;"
		"vpaddd ymm1, ymm7, ymm1;"
		"vmovdqu [%[integral] + 4*%[w]], ymm4;"
		"vmovdqu [%[integral] + 4*%[w] + 32], ymm1;"
		"vpmovzxwd ymm3, xmm2;"
		"vpaddd ymm3, ymm7, ymm3;"
		"vpaddd ymm3, ymm5, ymm3;"
		"vpermq ymm2, ymm2, 14;"
		"vpmovzxwd ymm2, xmm2;"
		"vpaddd ymm2, ymm7, ymm2;"
		"vpermq ymm7, ymm2, 255;"
		"vpaddd ymm2, ymm6, ymm2;"
		"vmovdqu [%[integral] + 4*%[w] + 64], ymm3;"
		"vmovdqu [%[integral] + 4*%[w] + 96], ymm2;"
		"vpsrlq ymm7, ymm7, 32;"
		"vpbroadcastd ymm7, xmm7;"
		"add %[ofs], 32;"
		"add %[integral], 128;"
		"cmp %[ofs], %[w];"
		"jl blockx;"
		"dec %[h];"
		"cmp %[h], 0;"
		"jne rowx;"
	"endx:"
		"vzeroupper;"
		:
		: [ofs] "r" (0), [img] "r" (img), [integral] "r" (integral), [w] "r" (w), [h] "r" (h)
		: "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "memory"
	);
}

static void cii(const uint8_t* img, uint32_t* integral, int w, int h)
{
	uint32_t acc = 0;
	for (int x = 0; x < w; x++)
	{
		integral[x] = acc + img[x];
		acc += img[x];
	}
	for (int y = 1, acc = 0; y < h; y++, acc = 0)
	for (int x = 0; x < w; x++)
	{
		int ofs = y*w + x;
		integral[ofs] = acc + img[ofs] + integral[ofs - w];
		acc += img[ofs];
	}
}

static void cii8(const uint8_t* img, uint32_t* integral, int w, int h)
{
	union
	{
		uint8_t bytes[8];
		uint64_t quad;
	} le;
	uint32_t acc = 0;
	for (int x = 0; x < w; x += 8)
	{
		le.quad = ((uint64_t*)(img + x))[0];
		for (int i = 0; i < 8; i++)
		{
			integral[x + i] = le.bytes[i] + acc;
			acc += le.bytes[i];
		}
	}
	for (int y = 1, acc = 0; y < h; y++, acc = 0)
	for (int x = 0; x < w; x += 8)
	{
		int ofs = y*w + x;
		le.quad = ((uint64_t*)(img + ofs))[0];
		for (int i = 0; i < 8; i++)
		{
			integral[ofs + i] = le.bytes[i] + acc + integral[ofs + i - w];
			acc += le.bytes[i];
		}
	}
}

static void testii(const char* name, void (*ii)(const uint8_t* img, uint32_t* integral, int w, int h))
{
	const int W = 4096;
	const int H = 4096;

	vector<uint8_t> img(W*H + 32); // >_>
	vector<uint32_t> integral(W*H + 32); // <_<
	for (int i = 0; i < img.size(); i++)
	{
		img[i] = (uint8_t)i;
	}

	auto expected = [](int x, int y)
	{
		uint32_t acc = 255*128;
		acc *= x/256;
		acc += ((x%256)*(x%256 + 1))/2;
		return (y + 1)*acc;
	};

	int times = 1000;
	auto s = high_resolution_clock::now();
	for (int i = times; i --> 0;)
	{
		ii(img.data(), integral.data(), W, H);
	}
	auto dt = duration<double, micro>(high_resolution_clock::now() - s);

	uint32_t errors = 0;
	for (int y = 0; y < H; y++)
	for (int x = 0; x < W; x++)
	if (integral[W*y + x] != expected(x, y))
	{
		errors++;
	}
	msprintf("'%s' x%i: %0.1lf us, %u errors", name, times, dt.count(), errors);
}

int main(int argn, char* argv[])
{
	testii("ez c", cii);
	testii("tryhard c", cii8);
	testii("sus avx", avxii);
	return 0;
}
