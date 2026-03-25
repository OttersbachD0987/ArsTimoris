#include <ArsTimoris/Game/Classes/ClassInstance.h>

ClassInstance::ClassInstance(void) {
    this->classType = "";
    this->level = 0;
}

ClassInstance::ClassInstance(std::string_view a_class, uint32_t a_level) {
    this->classType = a_class;
    this->level = a_level;
}