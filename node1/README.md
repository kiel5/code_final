Đề tài: thiết kế thiết bị đo và giám sát mức tiêu thụ nước trong hộ gia đình
Cấu trúc thư mục: main/: chứa code chính
                  html/: chứa web config wifi
                  common/: chứa các file thư viện
Hướng dẫn sử dụng: cài đặt môi trường ESP-IDF
                   build và flash code lên esp32 bằng lệnh: idf.py build, idf.py -p COMX flash, idf.py -p COMX monitor
các file thư viện: app_config: cấu hình wifi qua smartconfig hoặc access point
                   app_mqtt  : kết nối mqtt
                   app_ota   : update OTA
                   app_nvs_flash: ghi giá trị vào flash
                   http_server_app: chạy webserver phục vụ config wifi
                   input_iot và output_iot: cấu hình gpio
                   oled      : cấu hình oled
                   sensor    : tạo các task để đo, hiển thị, gửi... dữ liệu
                   partition.csv : phân vùng flash