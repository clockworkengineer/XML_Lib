#pragma once

#include <cstddef>
#include <memory_resource>
#include <vector>

namespace XML_Lib {

// Default arena size: use the CMake-controlled macro when available, otherwise 256 KB.
#if !defined(XML_LIB_ARENA_SIZE_KB)
#  define XML_LIB_ARENA_SIZE_KB 256
#endif

class XML_Arena
{
public:
  explicit XML_Arena(std::size_t initialSize = static_cast<std::size_t>(XML_LIB_ARENA_SIZE_KB) * 1024)
    : buffer(initialSize), resource(buffer.data(), buffer.size())
  {}

  void *allocate(std::size_t bytes, std::size_t alignment = alignof(std::max_align_t))
  {
    return resource.allocate(bytes, alignment);
  }

  std::pmr::memory_resource *memoryResource() noexcept
  {
    return &resource;
  }

  static XML_Arena *getCurrent() noexcept
  {
    return currentArena;
  }

  class ScopedCurrentArena
  {
  public:
    explicit ScopedCurrentArena(XML_Arena &arena) noexcept
      : previousArena(currentArena)
    {
      currentArena = &arena;
    }

    ~ScopedCurrentArena() noexcept
    {
      currentArena = previousArena;
    }

  private:
    XML_Arena *previousArena;
  };

  class ScopedDefaultResource
  {
  public:
    explicit ScopedDefaultResource(XML_Arena &arena) noexcept
      : previousResource(std::pmr::get_default_resource())
    {
      std::pmr::set_default_resource(arena.memoryResource());
    }

    ~ScopedDefaultResource() noexcept
    {
      std::pmr::set_default_resource(previousResource);
    }

  private:
    std::pmr::memory_resource *previousResource;
  };

private:
  std::vector<std::byte> buffer;
  std::pmr::monotonic_buffer_resource resource;
  static inline XML_Arena *currentArena = nullptr;
};

} // namespace XML_Lib
