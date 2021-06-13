#pragma once


#define GET_JSON_FLOAT_FIELD(js, field) (js[field].is_null() ? 0 : (js[field].is_string() ? atof(js[field].get<std::string>().c_str()) : js[field].get<double>()))
#define GET_JSON_NUM_FIELD(js, field) (js[field].is_null() ? 0 : (js[field].is_string() ? atol(js[field].get<std::string>().c_str()) : js[field].get<long>()))
#define GET_JSON_BOOL_FIELD(js, field) (js[field].is_null() ? 0 : (js[field].is_string() ? atol(js[field].get<std::string>().c_str()) : js[field].get<bool>()))
#define GET_JSON_STR_FIELD(js, field) js[field].is_null() ? "" : js[field].get<std::string>().c_str()

