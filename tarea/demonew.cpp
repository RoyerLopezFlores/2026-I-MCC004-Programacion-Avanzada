#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <new>
#include <vector>
#include <chrono>
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#endif
using TF = float;
using TD = double;
using TI = int;
using TC = char;
using TIDX = std::size_t;
using LTV = std::size_t;
using TSIZE = std::size_t;
using TBYTE = unsigned char;
#define BLOCK_FULL static_cast<char>(219)
#define BLOCK_EMPTY static_cast<char>(176)
namespace {

    struct AllocationRecord {
        void* ptr = nullptr;
        LTV size = 0;
        bool aligned = false;
    };

    constexpr LTV kMaxRecords = 4096;
    AllocationRecord g_records[kMaxRecords];

    LTV g_allocCount = 0;
    LTV g_freeCount = 0;
    LTV g_bytesRequested = 0;
    bool g_forceFail = false;

    void printHeader(const TC* title) {
        std::cout << "\n=== " << title << " ===\n";
    }

    void trackerAdd(void* ptr, std::size_t size, bool aligned) {
        if (!ptr) {
            return;
        }
        for (TIDX i = 0; i < kMaxRecords; ++i) {
            if (g_records[i].ptr == nullptr) {
                g_records[i].ptr = ptr;
                g_records[i].size = size;
                g_records[i].aligned = aligned;
                return;
            }
        }
    }

    void trackerRemove(void* ptr) {
        if (!ptr) {
            return;
        }
        for (TIDX i = 0; i < kMaxRecords; ++i) {
            if (g_records[i].ptr == ptr) {
                //std::cout << "[DBG_FREE] " << ptr << " (" << g_records[i].size << " bytes) liberado\n";
                g_records[i] = AllocationRecord{};
                
                return;
            }
        }
    }

    void reportLeaks(std::ostream& os = std::cout) {
        LTV leakCount = 0;
        LTV leakBytes = 0;
        for (TIDX i = 0; i < kMaxRecords; ++i) {
            if (g_records[i].ptr != nullptr) {
                ++leakCount;
                leakBytes += g_records[i].size;
            }
        }
        if (leakCount == 0) {
            os << "Leak report: sin fugas detectadas\n";
        } else {
            os << "Leak report: " << leakCount
               << " bloque(s), " << leakBytes << " bytes\n";
        }
    }

    void* rawAlignedAlloc(LTV alignment, LTV size) {
        if (size == 0) {
            size = 1;
        }
        #if defined(_MSC_VER) || defined(__MINGW32__)
            return _aligned_malloc(size, alignment);
        #elif defined(__cpp_aligned_new)
            void* p = nullptr;
            if (posix_memalign(&p, alignment, size) != 0) {
                return nullptr;
            }
            return p;
        #else
            return nullptr;
        #endif
    }

    void rawAlignedFree(void* ptr) {
        #if defined(_MSC_VER) || defined(__MINGW32__)
            _aligned_free(ptr);
        #else
            std::free(ptr);
        #endif
    }

}  // namespace

void* operator new(LTV size) {
	if (size == 0) {
		size = 1;
	}
	if (g_forceFail) {
		throw std::bad_alloc();
	}
	void* ptr = std::malloc(size);
	if (!ptr) {
		throw std::bad_alloc();
	}
	++g_allocCount;
	g_bytesRequested += size;
	trackerAdd(ptr, size, false);
	return ptr;
}

void operator delete(void* ptr) noexcept {
	if (ptr) {
		++g_freeCount;
		trackerRemove(ptr);
	}
	std::free(ptr);
}

void* operator new[](LTV size) {
	return ::operator new(size);
}

void operator delete[](void* ptr) noexcept {
	::operator delete(ptr);
}
// Delete operators with size parameter (C++14 and later)
// Depende del compilador, puedes ver cuanta memoria se libera
void operator delete(void* ptr, LTV) noexcept {
	::operator delete(ptr);
}

void operator delete[](void* ptr, LTV) noexcept {
	::operator delete[](ptr);
}

// void*: Es un puntero genérico, puede apuntar a cualquier tipo de dato.
// Nothrow no lanza excepciones, evita el std::bad_alloc y devuelve nullptr
void* operator new(LTV size, const std::nothrow_t&) noexcept {
	try {
		return ::operator new(size);
	} catch (...) {
		return nullptr;
	}
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
	::operator delete(ptr);
}

void* operator new[](LTV size, const std::nothrow_t&) noexcept {
	try {
		return ::operator new[](size);
	} catch (...) {
		return nullptr;
	}
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
	::operator delete[](ptr);
}
/* Solo es para el trace*/
void* operator new(LTV size, std::align_val_t align) {
	const LTV alignment = static_cast<LTV>(align);
	if (g_forceFail) {
		throw std::bad_alloc();
	}
    std::fprintf(stdout, "[DBG_NEW] aligned %zu bytes\n", alignment);
	void* ptr = rawAlignedAlloc(alignment, size);
	if (!ptr) {
		throw std::bad_alloc();
	}
	++g_allocCount;
	g_bytesRequested += size;
	trackerAdd(ptr, size, true);
	return ptr;
}

void operator delete(void* ptr, std::align_val_t) noexcept {
	if (ptr) {
		++g_freeCount;
		trackerRemove(ptr);
	}
	rawAlignedFree(ptr);
}

void* operator new(LTV size, const TC* file, int line) {
	void* ptr = ::operator new(size);
    // Es bajo nivel, por lo que usamos fprintf para imprimir en stderr
    // duda duda por que tambien malloc es bajo nivel.
	std::fprintf(stderr, "[DBG_NEW] %s:%d -> %zu bytes @ %p\n", file, line, size, ptr);
	return ptr;
}

void* operator new[](LTV size, const TC* file, TI line) {
	void* ptr = ::operator new[](size);
    // stderr = std::cerr, stdout = std::cout
	std::fprintf(stderr, "[DBG_NEW[]] %s:%d -> %zu bytes @ %p\n", file, line, size, ptr);
	return ptr;
}

void operator delete(void* ptr, const TC*, TI) noexcept {
	::operator delete(ptr);
}

void operator delete[](void* ptr, const TC*, TI) noexcept {
	::operator delete[](ptr);
}

#define DBG_NEW new (__FILE__, __LINE__)


// alignas hace que la estructura PoolParticle esté alineada a 32 bytes
struct alignas(32) PoolParticle {
	TF x = 0;
	TF y = 0;
	TF z = 0;
	static constexpr LTV kBlockBytes = 32;
    static constexpr LTV kMaxParticles = 16;

	struct alignas(std::max_align_t) PoolSlot {
		TBYTE bytes[kBlockBytes];
	};

    
	static bool used[kMaxParticles];
    static TIDX allocationSize[kMaxParticles];
	inline static PoolSlot pool[kMaxParticles] = {};

	static void* operator new(std::size_t size) {
		if (size > kBlockBytes) {
			throw std::bad_alloc();
		}
		for (TIDX i = 0; i < kMaxParticles; ++i) {
			if (!used[i]) {
				used[i] = true;
				return pool[i].bytes;
			}
		}
		throw std::bad_alloc();
	}

	static void operator delete(void* ptr) noexcept {
		for (TIDX i = 0; i < kMaxParticles; ++i) {
			if (ptr == pool[i].bytes) {
				used[i] = false;
				return;
			}
		}
	}
    
    static void* operator new[](std::size_t size) {
		// Por ahora dejamos sin restricciones
        LTV blocks = (size + kBlockBytes - 1) / kBlockBytes;
        //std::cout<< "Requesting " << size << " bytes, which is " << blocks << " blocks\n";
        //if (blocks > kMaxParticles) {
        //    throw std::bad_alloc();
        //}

        for(TIDX i = 0; i < kMaxParticles - blocks + 1; ++i) {
            bool canAllocate = true;
            // Hay memoria contigua ? 
            for (TIDX j = 0; j < blocks; ++j) {
                //std::cout<< kMaxParticles <<" Checking block " << (i + j) << ": " << (used[i + j] ? "used" : "free") << "\n";
                //if(i + j >= kMaxParticles) {
                //    canAllocate = false;
                //    throw std::bad_alloc();
                //}
                if (used[i + j]) {
                    canAllocate = false;
                    break;
                }
            }
            // Si hay reservamos devolvemos el puntero al primer bloque
            if (canAllocate) {
                for (TIDX j = 0; j < blocks; ++j) {
                    used[i + j] = true;
                }
                allocationSize[i] = blocks;
				return pool[i].bytes;
            }
        }
        throw std::bad_alloc();
    }
    static void operator delete[](void* ptr) noexcept {
        for (TIDX i = 0; i < kMaxParticles; ++i) {
			if (ptr == pool[i].bytes) {
                LTV blocks = allocationSize[i];
                for (TIDX j = 0; j < blocks; ++j) {
                    used[i + j] = false;
                    // 
                    //TIDX k = i + j;
                    //PoolParticle::operator delete(pool[k]); //(ptr + j * kBlockBytes);
                }
                return;
            }
        }
    }
    
    static void printPoolStatus(std::ostream& os = std::cout) {
        for (TIDX i = 0; i < kMaxParticles; ++i) {
            os << (used[i] ? BLOCK_FULL : BLOCK_EMPTY);
        }
        os << "\n";
    }
};

bool PoolParticle::used[PoolParticle::kMaxParticles] = {};
TIDX PoolParticle::allocationSize[PoolParticle::kMaxParticles] = {};


template <typename T>
class ArenaAllocator {
public:
	using value_type = T;

	ArenaAllocator(TBYTE* begin, LTV bytes)
		: begin_(begin), bytes_(bytes), offset_(0) {}

	template <typename U>
	ArenaAllocator(const ArenaAllocator<U>& other) noexcept
		: begin_(other.begin_), bytes_(other.bytes_), offset_(other.offset_) {}

	T* allocate(LTV n) {
		const LTV need = n * sizeof(T);
		const LTV align = alignof(T);
		LTV aligned = (offset_ + (align - 1)) & ~(align - 1);
		if (aligned + need > bytes_) {
			throw std::bad_alloc();
		}
		T* ptr = reinterpret_cast<T*>(begin_ + aligned);
		offset_ = aligned + need;
		return ptr;
	}

	void deallocate(T*, std::size_t) noexcept {}

	template <typename U>
	bool operator==(const ArenaAllocator<U>&) const noexcept { return true; }

	template <typename U>
	bool operator!=(const ArenaAllocator<U>&) const noexcept { return false; }

	TBYTE* begin_;
	LTV bytes_;
	LTV offset_;
	
	friend std::ostream& operator<<(std::ostream& os, const ArenaAllocator<T>& allocator) {
		for(TIDX i = 0; i < allocator.offset_; ++i) {
			os << BLOCK_FULL;
		}
		for (LTV i = allocator.offset_; i < allocator.bytes_; ++i) {
			os << BLOCK_EMPTY;
		}
		return os;
	}
	

};

template <typename T>
class CountingAllocator {
public:
	using value_type = T;
	static inline std::size_t allocCalls = 0;
	static inline std::size_t bytes = 0;

	CountingAllocator() = default;

	template <typename U>
	CountingAllocator(const CountingAllocator<U>&) noexcept {}

	T* allocate(std::size_t n) {
		allocCalls++;
		bytes += n * sizeof(T);
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	void deallocate(T* ptr, std::size_t) noexcept {
		::operator delete(ptr);
	}

	template <typename U>
	bool operator==(const CountingAllocator<U>&) const noexcept { return true; }

	template <typename U>
	bool operator!=(const CountingAllocator<U>&) const noexcept { return false; }
};

class SmallBlockAllocator {
public:
	SmallBlockAllocator() {
		for (TIDX i = 0; i < kBlocks; ++i) {
			free_[i] = true;
		}
	}

	void* allocate() {
		for (TIDX i = 0; i < kBlocks; ++i) {
			if (free_[i]) {
				free_[i] = false;
				return blocks_[i];
			}
		}
		throw std::bad_alloc();
	}

	void deallocate(void* ptr) {
		for (TIDX i = 0; i < kBlocks; ++i) {
			if (ptr == blocks_[i]) {
				free_[i] = true;
				return;
			}
		}
	}

private:
	static constexpr LTV kBlocks = 1024;
	alignas(std::max_align_t) unsigned char blocks_[kBlocks][32] = {};
	bool free_[kBlocks] = {};
};
template <typename T>
struct TinyNode {
	T v = 0;
	static inline SmallBlockAllocator alloc;

	static void* operator new(std::size_t size) {
		if (size > 32) {
			throw std::bad_alloc();
		}
		return alloc.allocate();
	}

	static void operator delete(void* ptr) noexcept {
		alloc.deallocate(ptr);
	}
};

template <typename T>
struct NodeWO {
	T v = 0;
};

class FrameAllocator {
public:
	void* alloc(std::size_t bytes, std::size_t align = alignof(std::max_align_t)) {
		std::size_t aligned = (offset_ + (align - 1)) & ~(align - 1);
		if (aligned + bytes > kCapacity) {
			return nullptr;
		}
		void* p = buffer_ + aligned;
		offset_ = aligned + bytes;
		return p;
	}

	void reset() {
		offset_ = 0;
	}

private:
	static constexpr LTV kCapacity = 4096;
	alignas(std::max_align_t) TBYTE buffer_[kCapacity] = {};
	LTV offset_ = 0;
};
// Simula un sistema embebido con presupuesto limitado de memoria
class EmbeddedBudgetAllocator {
public:
	static void* operator new(LTV size, const std::nothrow_t&) noexcept {
		if (used_ + size > kBudget) {
			return nullptr;
		}
		void* p = std::malloc(size);
		if (!p) {
			return nullptr;
		}
		used_ += size;
		return p;
	}

	static void operator delete(void* ptr) noexcept {
		std::free(ptr);
	}
	friend std::ostream& operator<<(std::ostream& os, const EmbeddedBudgetAllocator&) {
		for (LTV i = 0; i < used_; ++i) {
			os << BLOCK_FULL;
		}
		for (LTV i = used_; i < kBudget; ++i) {
			os << BLOCK_EMPTY;
		}
		return os;
	}

	static constexpr LTV kBudget = 128;
	static inline LTV used_ = 0;
};

struct EmbeddedPacket : EmbeddedBudgetAllocator {
	char payload[64] = {};
	
};

extern "C" {
	typedef void* (*ExternalAllocFn)(LTV bytes, void* ctx);
	typedef void (*ExternalFreeFn)(void* ptr, void* ctx);

	struct ExternalMemApi {
		ExternalAllocFn alloc;
		ExternalFreeFn free;
		void* ctx;
	};
}

void* externalAllocBridge(LTV bytes, void*) {
	return ::operator new(bytes);
}

void externalFreeBridge(void* ptr, void*) {
	::operator delete(ptr);
}

void demo1_memoryPool() {
	printHeader("1) Crear un memory pool");
	//std::vector<PoolParticle*> particles;
    //// Para simular error usar 17 a mas
	//for (int i = 0; i < 16; ++i) {
	//	particles.push_back(new PoolParticle());
	//}
    TIDX lengthP = 14;
    std::cout<< "Size PoolParticle: "<< sizeof(PoolParticle) << " bytes\n";
	std::cout<< "PoolParticle default\n";
	PoolParticle::printPoolStatus();
    PoolParticle* particles = new PoolParticle[lengthP];
    std::cout<< "PoolParticle after new PoolParticle["<< lengthP <<"]\n";
    PoolParticle::printPoolStatus();

    PoolParticle* custom = new PoolParticle();
	std::cout<< "PoolParticle after new custom\n";
	PoolParticle::printPoolStatus();
    //PoolParticle::printPoolStatus();
	//delete particles[0];
    //PoolParticle::printPoolStatus();
	//TIDX index = 1;
	
	//std::cout << "custom particle address: " << custom  << " " << &particles[index] << "\n";
	//delete custom;
	//std::cout << "custom particle deleted\n";
	//delete custom;
	
	
	//delete particles[1];
    delete[] particles;
	std::cout<< "PoolParticle after delete[] particles\n";
    PoolParticle::printPoolStatus();
	delete custom;
	std::cout<< "PoolParticle after delete custom\n";
	PoolParticle::printPoolStatus();

    std::cout << "\n";
}

void demo2_customAllocators() {
	printHeader("2) Implementar allocators personalizados");
	unsigned char arena[128] = {};
	using TAlloc = int;
	ArenaAllocator<TAlloc> arenaAlloc(arena, sizeof(arena));
	
	std::vector<TAlloc, ArenaAllocator<TAlloc>> v(arenaAlloc);
	std::cout << "arena: " << v.get_allocator() << "\n";
	v.push_back(10);
	std::cout << "arena: " << v.get_allocator() << "\n";
	v.push_back(20);
	std::cout << "arena: " << v.get_allocator() << "\n";
	v.push_back(50);
	std::cout << "arena: " << v.get_allocator() << "\n";
	v.push_back(40);
	std::cout << "arena: " << v.get_allocator() << "\n";
	std::cout << "vector en arena: " << v[0] << ", " << v[1] << "\n";
	printf("--- Otra arena ---\n");
	std::cout << "arena: " << arenaAlloc << "\n";
	TAlloc* p1 = arenaAlloc.allocate(1);
	std::cout << "arena: " << arenaAlloc << "\n";
	std::cout << "p1=" << p1 << "\n";
}

void demo3_memoryAccounting() {
	printHeader("3) Registrar cuanta memoria se reserva");
	int* p = new int[40];
	std::cout << "allocs=" << g_allocCount << " frees=" << g_freeCount
			  << " bytes=" << g_bytesRequested << "\n";
	delete[] p;
}

void demo4_leakDetection() {
	printHeader("4) Detectar memory leaks");
	auto genLeak = []() {
		char* leak = DBG_NEW char[16];
		std::strcpy(leak, "leak demo");
	};
	genLeak();
	//char* leaked = DBG_NEW char[32];
	
	//std::strcpy(leaked, "leak temporal");
	::reportLeaks();
	//delete[] leaked;
	//reportLeaks();
}

void demo5_specialAlignment() {
	printHeader("5) Alinear memoria de forma especial");
	struct alignas(64) SimdBlock {
		TI data[16];
	};
	SimdBlock* p = new SimdBlock();
	std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(p);
	std::cout << "direccion % 64 = " << (addr % 64) << "\n";
	delete p;
}

void demo6_specificRegion() {
	printHeader("6) Reservar memoria de una region especifica");
	unsigned char region[128] = {};
	ArenaAllocator<int> regionAlloc(region, sizeof(region));
	std::cout << "region arena: " << regionAlloc << "\n";
	int* x = regionAlloc.allocate(4);
	std::cout << "region arena: " << regionAlloc << "\n";
	for (int i = 0; i < 4; ++i) {
		x[i] = i * 5;
	}
	std::cout << "region[3]=" << x[3] << "\n";
}

void demo7_manySmallAllocations() {
	printHeader("7) Optimizar muchas reservas pequenas");
	const TI tamNodes = 1000;
	auto start_time = std::chrono::high_resolution_clock::now();
	TinyNode<TI>* nodes[tamNodes] = {};
	for (int i = 0; i < tamNodes; ++i) {
		nodes[i] = new TinyNode<TI>();
		nodes[i]->v = i;
	}
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
	std::cout<< elapsed.count() << " ms para crear " << tamNodes << " TinyNode\n";
	auto start_time_wo = std::chrono::high_resolution_clock::now();
	NodeWO<TI>* nodesWO[tamNodes] = {};
	for (int i = 0; i < tamNodes; ++i) {
		nodesWO[i] = new NodeWO<TI>();
		nodesWO[i]->v = i;
	}
	auto end_time_wo = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed_wo = end_time_wo - start_time_wo;
	std::cout<< elapsed_wo.count() << " ms para crear " << tamNodes << " NodeWO\n";
	std::cout << "TinyNode[99]=" << nodes[tamNodes - 1]->v << " (sin malloc por nodo)\n";
	for (int i = 0; i < tamNodes; ++i) {
		delete nodes[i];
		delete nodesWO[i];
	}
}

void demo8_gameMemorySystem() {
	printHeader("8) Sistema de memoria para videojuegos (frame allocator)");
	FrameAllocator frame;
	for (int frameIndex = 0; frameIndex < 3; ++frameIndex) {
		void* p1 = frame.alloc(128, 16);
		void* p2 = frame.alloc(256, 16);
		std::cout << "frame " << frameIndex << " ptrs: " << p1 << " " << p2 << "\n";
		frame.reset();
	}
}

void demo9_embeddedControl() {
	printHeader("9) Control de memoria en sistemas embebidos");
	std::cout<< "Size EmbeddedPacket:	"<< sizeof(EmbeddedPacket) << " bytes\n";
	std::cout<< "Memory packet: 		"<< EmbeddedPacket::kBudget << " bytes\n";
	EmbeddedPacket* p1 = new (std::nothrow) EmbeddedPacket();
	EmbeddedPacket* p2 = new (std::nothrow) EmbeddedPacket();
	EmbeddedPacket* p3 = new (std::nothrow) EmbeddedPacket();
	std::cout << "p1=" << (p1 != nullptr)
			  << " p2=" << (p2 != nullptr)
			  << " p3=" << (p3 != nullptr) << " (presupuesto limitado)\n";
	delete p1;
	delete p2;
	delete p3;
}
/*
void demo10_externalApiIntegration() {
	printHeader("10) Integrar C++ con API externa de memoria");
	ExternalMemApi api{externalAllocBridge, externalFreeBridge, nullptr};
	void* raw = api.alloc(96, api.ctx);
	std::memset(raw, 0xAB, 96);
	std::cout << "API externa asigno 96 bytes en " << raw << "\n";
	api.free(raw, api.ctx);
}

void demoExtra_countingAllocator() {
	printHeader("Extra) Counting allocator STL");
	std::vector<int, CountingAllocator<int>> v;
	for (int i = 0; i < 40; ++i) {
		v.push_back(i * 2);
	}
	std::cout << "alloc calls STL=" << CountingAllocator<int>::allocCalls
			  << " bytes STL=" << CountingAllocator<int>::bytes << "\n";
}
*/
int main() {
    std::cout<< "Unidad basica de memoria(unsigned char) : "<< sizeof(unsigned char) << " bytes\n";
    std::cout<< "Tamaño (TF)"<< sizeof(TF) << " bytes\n";
    //std::cout<< "Unidad basica de memoria(unsigned short): "<< sizeof(unsigned short) << " bytes\n";
	demo1_memoryPool();
	demo2_customAllocators();
	demo3_memoryAccounting();
	demo4_leakDetection();
	demo5_specialAlignment();
	demo6_specificRegion();
	demo7_manySmallAllocations();
	demo8_gameMemorySystem();
	demo9_embeddedControl();
	/*
	demo10_externalApiIntegration();
	demoExtra_countingAllocator();
	*/
	std::cout << "\nResumen global -> allocs: " << g_allocCount
			  << ", frees: " << g_freeCount
			  << ", bytes solicitados: " << g_bytesRequested << "\n";
	return 0;
}
