#pragma once


#ifdef MOD_MANAGER_API_EXPORT
    #define MOD_MANAGER_API __declspec(dllexport)
#else
    #define MOD_MANAGER_API __declspec(dllimport)
#endif