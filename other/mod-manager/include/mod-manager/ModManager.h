#pragma once


class ModManager
{
public:
    void OnProcessAttach();
    void OnProcessDetach();
    void OnThreadAttach();
    void OnThreadDetach();
};