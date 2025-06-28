Wellcome to ThaiShell!
![image](https://github.com/user-attachments/assets/c504a0ac-a584-4f11-9bf2-0a5ebd0dac2b)
To run Thai shell :

please built in by : 
Bước 1: Tạo bản sao của dự án trên máy tính của bạn.
git clone https://github.com/HaiAu2501/Operating-System-Projects.git
Bước 2: Tạo folder build cho CMake và chuyển đến thư mục này.
mkdir build
cd build
Bước 3: Sử dụng CMake để tạo file Makefile hoặc Visual Studio project.
cmake ..
Bước 4: Biên dịch mã nguồn bằng trình biên dịch C++ mà bạn đã cài đặt.
cmake --build . --target install --config Debug
Bước 5: Chạy chương trình Operating-System-Project.exe từ thư mục install. Bạn nên nhập lệnh help để xem danh sách các lệnh hỗ trợ.
cd install
Operating-System-Project.exe
Nếu không biên dịch thì cũng không sao, bạn có thể tải file thực thi từ đây. Nếu chạy bị lỗi, hãy cài đặt Visual C++ Redistributable.
