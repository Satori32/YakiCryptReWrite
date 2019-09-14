#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <random>
#include <iomanip>
#include <string>

template<class T> using DType = std::vector<T>;

//https://ja.wikipedia.org/wiki/MD5

typedef std::vector<bool> BType;
typedef std::vector<std::uint32_t> DTypeU32;
typedef std::vector<std::uint8_t> Bytes;

template<class T>
T BitToUIntN(const BType& B) {
	std::size_t BitSize = std::numeric_limits<T>::digits;
	std::size_t ByteBit = std::numeric_limits<std::uint8_t>::digits;
	std::size_t ByteSize = BitSize / ByteBit;
	T R = 0;

	for (std::size_t i = 0; i < B.size() / ByteBit; i++) {
		for (std::size_t j = 0; j < ByteBit; j++) {
			std::size_t idx = 8 * i + j;

			R |= (B[idx] ? 1ull : 0) << (((ByteBit - 1) - j) + (i * ByteBit));
		}
	}
	return R;
}

DTypeU32 MD5(const Bytes& Data) {

	BType Bits;


	auto UIntNToBit = [](const auto& N)-> BType {
		std::size_t BitSize = std::numeric_limits<decltype(N)>::digits;
		std::size_t ByteBit = std::numeric_limits<std::uint8_t>::digits;
		std::size_t ByteSize = BitSize / ByteBit;

		BType B;

		for (std::size_t i = 0; i < ByteSize; i++) {
			for (std::size_t j = 0; j < ByteBit; j++) {
				std::size_t BS = i * ByteBit;
				std::size_t BB = (ByteBit - 1 - j);

				B.push_back(N & (1ull << (BS + BB)));
			}
		}

		return B;
	};

	auto BitToUInt32 = [](const BType& B) -> std::uint32_t {
		std::uint32_t R = 0;
		for (std::size_t i = 0; i < B.size() / 8; i++) {
			for (std::size_t j = 0; j < 8; j++) {
				std::size_t idx = 8 * i + j;
				R |= (B[idx] ? 1 : 0) << ((7 - j) + ((8 * i)));
			}
		}
		return R;
	};

	auto LeftRotate = [](const std::uint32_t& X, const std::int32_t& C)->std::uint32_t {
		return (X << C) | (X >> (32 - C));
	};
	auto makeK = [](const std::uint32_t& i) -> std::uint32_t {
		return static_cast<std::uint32_t>(std::floor(((1ull << 32)) * std::abs(std::sin(i + 1.0))));
	};

	/* * /
	DType K;

	for (std::size_t i = 0; i < 64; i++) {
		K.push_back(makeK(i));
	}
	/**/
	DTypeU32 K =
	{
		0xD76AA478, 0xE8C7B756, 0x242070DB,0xC1BDCEEE,
		0xF57C0FAF, 0x4787C62A, 0xA8304613,0xFD469501,
		0x698098D8, 0x8B44F7AF, 0xFFFF5BB1,0x895CD7BE,
		0x6B901122, 0xFD987193, 0xA679438E,0x49B40821,
		0xF61E2562, 0xC040B340, 0x265E5A51,0xE9B6C7AA,
		0xD62F105D, 0x02441453, 0xD8A1E681,0xE7D3FBC8,
		0x21E1CDE6, 0xC33707D6, 0xF4D50D87,0x455A14ED,
		0xA9E3E905, 0xFCEFA3F8, 0x676F02D9,0x8D2A4C8A,
		0xFFFA3942, 0x8771F681, 0x6D9D6122,0xFDE5380C,
		0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60,0xBEBFBC70,
		0x289B7EC6, 0xEAA127FA, 0xD4EF3085,0x04881D05,
		0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8,0xC4AC5665,
		0xF4292244, 0x432AFF97, 0xAB9423A7,0xFC93A039,
		0x655B59C3, 0x8F0CCC92, 0xFFEFF47D,0x85845DD1,
		0x6FA87E4F, 0xFE2CE6E0, 0xA3014314,0x4E0811A1,
		0xF7537E82, 0xBD3AF235, 0x2AD7D2BB,0xEB86D391,
	};
	/**/

	const DTypeU32 s =
	{
	 7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
	 5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	 4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	 6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
	};

	for (std::size_t i = 0; i < Data.size(); i++) {
		auto B = UIntNToBit(Data[i]);
		Bits.insert(Bits.end(), B.begin(), B.end());

	}

	Bits.push_back(true);
	std::uint64_t IML = Data.size() * 8;
	while ((Bits.size() % 512) != 448) {
		Bits.push_back(false);
	}

	for (std::size_t i = 0; i < 2; i++) {
		for (std::int32_t j = 0; j < 4; j++) {
			for (std::size_t k = 0; k < 8; k++) {
				Bits.push_back(IML & ((1ull << ((j * 8) + (7 - k))) * i));
			}
		}
	}

	//A、B、C、Dの初期値
	std::uint32_t a0 = 0x67452301; // A
	std::uint32_t b0 = 0xEFCDAB89; // B
	std::uint32_t c0 = 0x98BADCFE; // C
	std::uint32_t d0 = 0x10325476; // D

	for (std::size_t j = 0; j < (Bits.size() / 512); j++) {
		DTypeU32 M;
		for (std::size_t k = 0; k < 16; k++) {
			BType T{ Bits.begin() + (k * 32) + (j * 512) ,Bits.begin() + ((k + 1) * 32) + (j * 512) };
			M.push_back(BitToUIntN<std::uint32_t>(T));
		}

		std::uint32_t A = a0;
		std::uint32_t B = b0;
		std::uint32_t C = c0;
		std::uint32_t D = d0;

		std::uint32_t F = 0;
		std::int32_t g = 0;

		for (std::size_t i = 0; i < 64; i++) {
			if (0 <= i && i <= 15) {
				F = (B & C) | ((~B) & D);
				g = i;
			}
			else if (16 <= i && i <= 31) {
				F = (D & B) | ((~D) & C);
				g = ((5 * i) + 1) % 16;
			}
			else if (32 <= i && i <= 47) {
				F = (B ^ C) ^ D;
				g = ((3 * i) + 5) % 16;
			}
			else if (48 <= i && i <= 63) {
				F = C ^ (B | (~D));
				g = (7 * i) % 16;
			}

			F = F + A + K[i] + M[g];
			A = D;
			D = C;
			C = B;


			B = B + LeftRotate(F, s[i]);
		}
		a0 = a0 + A;
		b0 = b0 + B;
		c0 = c0 + C;
		d0 = d0 + D;
	}
	DTypeU32 md5 = { a0,b0,c0,d0 };

	return md5;
}

template<class T>
DType<T> GetInput(std::size_t N, unsigned int S = 0) {
	DType<T> R;
	std::mt19937 mt(S);
	for (std::size_t i = 0; i < N; i++) {
		R.push_back(i);
	}
	//std::shuffle(R.begin(), R.end(), mt);
	std::shuffle(R.begin(), R.end(), mt);
	//std::reverse(R.begin(), R.end());
	return R;
}

template<class T,class U>
DType<T> SplitDigit(std::uintmax_t N,const DType<U>& C) {
	DType<T> D;

	if (N==0)D.push_back(C[0]);

	while (N != 0) {
		
		D.push_back(C[N % C.size()]);
		N /= C.size();
	}
	std::reverse(D.begin(), D.end());
	return D;
}
template<class T,class U>
std::uintmax_t StickDigit(const DType<T>& D, const DType<U>& C) {
	std::uintmax_t N = 0;

	for (auto& o : D) {
		N *= C.size();
		auto it = std::find(C.begin(), C.end(), o);
		N += std::distance(C.begin(),it);
	}
	return N;
}

template<class T,class U>
DType<T>ReversibleHashEncode(const DType<T>& D,const DType<U>& C) {
	DType<T> R;
	for (auto& o : D) {
		R.push_back(std::numeric_limits<T>::max());//bug maker.
		auto TT = SplitDigit<T>(o, C);
		R.insert(R.end(),TT.begin(), TT.end());
	}
	return R;
}
template<class X,class T,class U>
DType<X>ReversibleHashDecode(const DType<T>& D,const DType<U>& C) {
	DType<X> R;
	DType<T> TT;
	std::uintmax_t N = 0;
	bool F = true;
	for (auto& o : D) {
		if (o == std::numeric_limits<T>::max()) {//bug maker.
			if (F) {
				F = false;
				continue;
			}
			N = StickDigit(TT, C);
			R.push_back(N);
			TT.clear();
		}
		else {
			TT.push_back(o);
		}
	}
	if (TT.size()) {
		N = StickDigit(TT, C);
		R.push_back(N);
	}
	return R;
}

template<class T>
DType<T> MakeCryptoNumber(const DType<T>& D) {
	DType<T> R;

	for (auto& o : D) {
		auto it = std::find(R.begin(), R.end(),o);
		if (it == R.end()) {
			R.push_back(o);
		}
	}
	return R;
}
template<class T,class U>
DType<T> ConvertDType(const DType<U>& In) {
	DType<T> R;
	for (auto& o : In) {
		R.push_back(o);
	}
	return R;
}

std::string GetInput() {

	std::string R;

	std::getline(std::cin, R);

	return R;
}

template<class T>
DTypeU32 YakiCrypto(const DType<T>& D) {

	auto C = MakeCryptNumber(D);
	auto RME = ReversibleHashEncode(D, C);
	return MD5({ RME.begin(),RME.end() });
}
template<class T,class U>
DTypeU32 YakiCrypto(const DType<T>& D,const DType<U>& C) {

	//auto C = MakeCryptNumber(D);
	auto RME = ReversibleHashEncode(D, C);
	return MD5({ RME.begin(),RME.end() });
}
#define DefValue
int main() {

	std::cout << "I am \"YakiCrypto\" the StringCrypter." << std::endl << std::endl;

	std::string S;
	std::cout << "Start Input. IF like a freeze. keyin By Input Device." << std::endl;
	std::cout << "Input:";
	std::getline(std::cin, S);
	std::cout << std::endl << "...Input Aceppted!" << std::endl;
	auto C = MakeCryptoNumber(DType<std::uint8_t>{S.begin(), S.end()});

	DTypeU32 RM = YakiCrypto<std::uint8_t>(DType<std::uint8_t>{S.begin(), S.end()},C);

	std::cout << std::endl << std::hex << std::setfill('0') << std::setw(8);
	for (auto& o : RM) { std::cout << o << ' '; }
	std::cout << std::endl;

	return 0;
}
/** /
int main() {

	std::string M = "yakicript";
//	auto M = GetInput();
	
	//auto M = GetInput<std::uintmax_t>(10);	
	for (auto& o : M) { std::cout << static_cast<std::intmax_t>(o) << ' '; }
	std::cout << std::endl;
	auto C = MakeCryptNumber<std::uint8_t>({ M.begin(),M.end() });
	for (auto& o : C) { std::cout << static_cast<std::intmax_t>(o) << ' '; }
	std::cout << std::endl;

	auto RHE = ReversibleHashEncode(DType<std::uint8_t>{ M.begin(),M.end() }, C);
	for (auto& o : RHE) { std::cout << static_cast<std::intmax_t>(o) << ' '; }
	std::cout << std::endl;
	auto U8V = ConvertDType<std::uint8_t>(RHE);
	DTypeU32 md = MD5(U8V);

	for(auto &o:md){ std::cout <<std::hex<<std::setfill('0')<<std::setw(8)<< static_cast<std::intmax_t>(o) << ' '; }
	std::cout << std::endl;

	auto RHD = ReversibleHashDecode<std::uintmax_t>(RHE, C);
	for (auto& o : RHD) { std::cout<<std::dec << static_cast<std::intmax_t>(o) << ' '; }
	std::cout << std::endl;

	return 0;
}
/**/
/** /
int main() {
	DType<int> C;

	for (std::size_t i = 0; i < 11; i++) {
		C.push_back(i);
	}

	auto R = SplitDigit<std::uint8_t>(0x123456789,C);
	auto N = StickDigit(R,C);

	for (auto& o : R) { std::cout << static_cast<std::intmax_t>(o) << ' '; }
	std::cout << std::endl;

	auto RHE = ReversibleHashEncode(R, C);
	auto RHD = ReversibleHashDecode<std::uintmax_t>(RHE, C);
	for (auto& o : RHD) { std::cout << static_cast<std::intmax_t>(o) << ' '; }
	std::cout << std::endl;
	N= StickDigit(RHD,C);
	return 0;
}
/**/