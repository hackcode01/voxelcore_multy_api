#pragma once

#include <filesystem>
#include <iterator>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "typedefs.hpp"
#include "data/dv.hpp"
#include "util/Buffer.hpp"
#include "path.hpp"

namespace io {
    class Device;

    /// @brief Set device for the entry-point
    void set_device(const std::string& name, std::shared_ptr<Device> device);
    
    /// @brief Remove device by entry-point
    void remove_device(const std::string& name);
    
    /// @brief Get device by entry-point
    std::shared_ptr<Device> get_device(const std::string& name);
    
    /// @brief Get device by entry-point or throw exception
    Device& require_device(const std::string& name);

    /// @brief Create subdevice for the entry-point
    /// @param name subdevice entry-point
    /// @param parent parent device entry-point
    /// @param root root path for the subdevice
    void create_subdevice(
        const std::string& name, const std::string& parent, const Path& root
    );

    /// @brief Read-only random access file
    class rafile {
        std::unique_ptr<std::istream> file;
        size_t filelength;
    public:
        rafile(const Path& filename);
        rafile(std::unique_ptr<std::istream> file, size_t length);

        void seekg(std::streampos pos);
        void read(char* buffer, std::streamsize size);
        size_t length() const;
    };

    class directory_iterator_impl {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = Path;
        using difference_type = std::ptrdiff_t;
        using pointer = Path*;
        using reference = Path&;

        directory_iterator_impl(
            PathsGenerator& generator, const Path& folder, bool end = false
        )
            : generator(generator), folder(folder), isend(end) {
            if (!isend && this->generator.next(current)) {
                isend = false;
                current = folder / current;
            } else {
                isend = true;
            }
        }

        reference operator*() {
            return current;
        }

        pointer operator->() {
            return &current;
        }

        directory_iterator_impl& operator++() {
            if (isend) {
                return *this;
            }
            if (generator.next(current)) {
                current = folder / current;
            } else {
                isend = true;
            }
            return *this;
        }

        bool operator==(const directory_iterator_impl& other) const {
            return isend == other.isend;
        }

        bool operator!=(const directory_iterator_impl& other) const {
            return !(*this == other);
        }
    private:
        PathsGenerator& generator;
        Path folder;
        Path current;
        bool isend = false;
    };

    class directory_iterator {
        std::unique_ptr<PathsGenerator> generator;
        Path folder;
    public:
        directory_iterator(const Path& folder);

        directory_iterator_impl begin() {
            return directory_iterator_impl(*generator, folder);
        }

        directory_iterator_impl end() {
            return directory_iterator_impl(*generator, "", true);
        }
    };

    /// @brief Write bytes array to the file without any extra data
    /// @param file target file
    /// @param data data bytes array
    /// @param size size of data bytes array
    bool write_bytes(const io::Path& file, const ubyte* data, size_t size);

    /// @brief Write string to the file
    bool write_string(const io::Path& file, std::string_view content);

    /// @brief Write dynamic data to the JSON file
    /// @param nice if true, human readable format will be used, otherwise
    /// minimal
    bool write_json(
        const io::Path& file, const dv::value& obj, bool nice = true
    );

    /// @brief Write dynamic data to the binary JSON file
    /// (see src/coders/binary_json_spec.md)
    /// @param compressed use gzip compression
    bool write_binary_json(
        const io::Path& file,
        const dv::value& obj,
        bool compressed = false
    );

    /// @brief Open file for writing
    /// @throw std::runtime_error if file cannot be opened
    std::unique_ptr<std::ostream> write(const io::Path& file);

    /// @brief Open file for reading 
    /// @throw std::runtime_error if file cannot be opened
    std::unique_ptr<std::istream> read(const io::Path& file);

    /// @brief Read bytes array from the file
    bool read(const io::Path& file, char* data, size_t size);
    util::Buffer<ubyte> read_bytes_buffer(const Path& file);
    std::unique_ptr<ubyte[]> read_bytes(const Path& file, size_t& length);
    std::vector<ubyte> read_bytes(const Path& file);

    /// @brief Read string from the file
    std::string read_string(const Path& file);

    /// @brief Read JSON or BJSON file
    /// @param file *.json or *.bjson file
    dv::value read_json(const Path& file);
    
    /// @brief Read BJSON file
    dv::value read_binary_json(const Path& file);
    
    /// @brief Read TOML file
    /// @param file *.toml file
    dv::value read_toml(const Path& file);

    /// @brief Read list of strings from the file
    std::vector<std::string> read_list(const io::Path& file);

    /// @brief Check if path is a regular file 
    bool is_regular_file(const io::Path& file);

    /// @brief Check if path is a directory
    bool is_directory(const io::Path& path);

    /// @brief Check if file or directory exists
    bool exists(const io::Path& path);

    /// @brief Create directory
    bool create_directory(const io::Path& file);

    /// @brief Create directories recursively
    bool create_directories(const io::Path& file);

    /// @brief Remove file or empty directory
    bool remove(const io::Path& file);

    /// @brief Copy src file to dst file
    /// @param src source file path
    /// @param dst destination file path
    /// @return true if success
    bool copy(const io::Path& src, const io::Path& dst);

    /// @brief Copy all files and directories in the folder recursively
    uint64_t copy_all(const io::Path& src, const io::Path& dst);

    /// @brief Remove all files and directories in the folder recursively
    uint64_t remove_all(const io::Path& file);

    /// @brief Get file size in bytes 
    size_t file_size(const io::Path& file);

    /// @brief Get file last write time timestamp
    file_time_type_t last_write_time(const io::Path& file);

    std::filesystem::path resolve(const io::Path& file);

    /// @brief Check if file is one of the supported data interchange formats
    bool is_data_file(const io::Path& file);

    /// @brief Check if file extension is one of the supported data interchange formats
    bool is_data_interchange_format(const std::string& ext);
    
    dv::value read_object(const Path& file);
}
