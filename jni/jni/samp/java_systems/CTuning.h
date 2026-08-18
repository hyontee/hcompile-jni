//
// Created by Error on 09.06.2025.
//

#ifndef CRIMINAL_MOSCOW_CTUNING_H
#define CRIMINAL_MOSCOW_CTUNING_H


class CTuning {
public:
    static void ChangeValue(int type, int value);
    static int GetValue(int type);
    static bool IsAvailable(int type);
};


#endif //CRIMINAL_MOSCOW_CTUNING_H
