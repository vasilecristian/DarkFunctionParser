#ifndef DFP_COMMON_H
#define DFP_COMMON_H
namespace dfp
{
    enum PARSE_RESULT
    {
        OK = 0,
        ERROR_COULDNT_OPEN,
        ERROR_INVALID_FILE_SIZE,
        ERROR_PARSING_FAILED,
        ERROR_MISSING_NODE,
        ERROR_IMAGE_PATHNAME_WRONG,
        ERROR_SPRITE_PATHNAME_WRONG,
        ERROR_ANIMATIONS_VER_MISSING,
        ERROR_NAME_WRONG,
        ERROR_NUMERIC_ATTRIBUTE_WRONG,
        ERROR_ROOT_MISSING,
    };

}// namespace dfp

#endif //DFP_COMMON_H