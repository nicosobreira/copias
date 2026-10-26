add_project_library(mygit_core SRC_DIR src)

add_project_executable(mygit SRC_DIR app)

find_package(OpenSSL 3.0 REQUIRED)

target_link_libraries(mygit_core PUBLIC commom OpenSSL::Crypto)
target_link_libraries(mygit PRIVATE mygit_core)
