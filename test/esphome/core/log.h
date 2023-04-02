// Fake Log for Local Testing

namespace esphome
{

#define ESP_LOGE(tag, format, ...)            \
    do                                        \
    {                                         \
        std::string str = "";                 \
        str += format;                        \
        str += "\n";                          \
        printf((str.c_str()), ##__VA_ARGS__); \
    } while (0);

#define ESP_LOGW(tag, format, ...)            \
    do                                        \
    {                                         \
        std::string str = "";                 \
        str += format;                        \
        str += "\n";                          \
        printf((str.c_str()), ##__VA_ARGS__); \
    } while (0);

#define ESP_LOGV(tag, format, ...)            \
    do                                        \
    {                                         \
        std::string str = "";                 \
        str += format;                        \
        str += "\n";                          \
        printf((str.c_str()), ##__VA_ARGS__); \
    } while (0);

} // namespace esphome
