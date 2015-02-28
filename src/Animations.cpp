#include "DarkFunctionParser/Animations.h"
#include "DarkFunctionParser/Sprite.h"

#include <tinyxml/tinyxml.h>

#include <cstdint>
#include <sstream>

namespace dfp
{

    Animations::Animations()
        : m_errorText("")
        , m_animationPath("")
        , m_spriteFileName("")
        , m_ver("")
    {}

    std::string Animations::GetSpriteFileName(bool onlyFileName)
    { 
        if (onlyFileName)
            return m_spriteFileName;

        std::string spritePathFileName = "";
        if (!m_animationPath.empty())
            spritePathFileName = m_animationPath;
        if (*m_animationPath.rbegin() != '/')
            spritePathFileName += "/";

        spritePathFileName += m_spriteFileName;

        return spritePathFileName;
    }

    std::string Animations::GetErrorText(){ return m_errorText; }

    std::shared_ptr<Anim> Animations::GetAnim(const std::string& animName)
    {
        auto it = m_anim.find(animName);

        if (it == m_anim.end())
            return nullptr;

        return std::make_shared<Anim>(m_anim[animName]);
    }

    PARSE_RESULT Animations::ParseFile(const std::string &fileName)
    {
        PARSE_RESULT errorsCode = PARSE_RESULT::OK;

        int lastSlash = fileName.find_last_of("/");

        // Get the directory of the file using substring.
        if (lastSlash > 0)
        {
            m_animationPath = fileName.substr(0, lastSlash + 1);
        }

        char* fileText;
        int fileSize;

        // Open the file for reading.
#ifdef USE_SDL2_LOAD
        SDL_RWops * file = SDL_RWFromFile(fileName.c_str(), "rb");
#else
        FILE *file = fopen(fileName.c_str(), "rb");
#endif

        // Check if the file could not be opened.
        if (!file)
        {
            errorsCode = PARSE_RESULT::ERROR_COULDNT_OPEN;
            return errorsCode;
        }

        // Find out the file size.	
#ifdef USE_SDL2_LOAD
        fileSize = file->size(file);
#else
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);
#endif

        // Check if the file size is valid.
        if (fileSize <= 0)
        {
            errorsCode = PARSE_RESULT::ERROR_INVALID_FILE_SIZE;
            return errorsCode;
        }

        // Allocate memory for the file and read it into the memory.
        fileText = new char[fileSize + 1];
        fileText[fileSize] = 0;
#ifdef USE_SDL2_LOAD
        file->read(file, fileText, 1, fileSize);
#else
        fread(fileText, 1, fileSize, file);
#endif

#ifdef USE_SDL2_LOAD
        file->close(file);
#else
        fclose(file);
#endif

        // Copy the contents into a C++ string and delete it from memory.
        std::string text(fileText, fileText + fileSize);
        delete[] fileText;

        return ParseText(text);
    }

    PARSE_RESULT Animations::ParseText(const std::string &text)
    {
        // Create a tiny xml document and use it to parse the text.
        TiXmlDocument doc;
        doc.Parse(text.c_str());

        // Check for parsing errors.
        if (doc.Error())
        {
            m_errorText = doc.ErrorDesc();
            return PARSE_RESULT::ERROR_PARSING_FAILED;
        }

        TiXmlNode *imgNode = doc.FirstChild("animations");
        if (!imgNode)
        {
            m_errorText = "Cannot find node <animations> !";
            return PARSE_RESULT::ERROR_MISSING_NODE;
        }

        TiXmlElement* imgElem = imgNode->ToElement();

        // Read the map attributes.
        m_spriteFileName = imgElem->Attribute("spriteSheet");
        if (m_spriteFileName.empty())
        {
            m_errorText = "Cannot find attribute 'spriteSheet' or the value is empty!";
            return PARSE_RESULT::ERROR_SPRITE_PATHNAME_WRONG;
        }

        m_ver = imgElem->Attribute("ver");
        if (m_ver.empty())
        {
            m_errorText = "Cannot find attribute 'ver' or the value is empty!";
            return PARSE_RESULT::ERROR_ANIMATIONS_VER_MISSING;
        }

        const TiXmlNode *node = imgElem->FirstChild();

        if (!node)
        {
            m_errorText = "The <spriteSheet> node does not have child nodes!";
            return PARSE_RESULT::ERROR_SPRITE_PATHNAME_WRONG;
        }

        while (node)
        {
            if (strcmp(node->Value(), "anim") == 0)
            {
                std::shared_ptr<Anim> anim = std::make_shared<Anim>();

                PARSE_RESULT result = anim->ParseXML(node);
                if (result == PARSE_RESULT::OK)
                {
                    m_anim[anim->GetName()] = anim;
                }
                else
                {
                    m_errorText = "Parsing <anim> Failed! >> " + anim->GetErrorText();
                    return result;
                }
            }

            node = node->NextSibling();
        }

        return PARSE_RESULT::OK;
    }

  





    Anim::Anim() : m_errorText("")
        , m_name("")
        , m_loops(0)
        , m_currentCellIndex(0)
        , m_timestampLastChange(0)
    {}

    Anim::Anim(const Anim &obj): m_currentCellIndex(0)
                               , m_timestampLastChange(0)
    {
        m_errorText = obj.m_errorText;
        m_name = obj.m_name;
        m_loops = obj.m_loops;
        m_cell = obj.m_cell;
    }

    Anim::Anim(const std::shared_ptr<Anim> obj) : m_currentCellIndex(0)
                                                , m_timestampLastChange(0)
    {
        m_errorText = obj->m_errorText;
        m_name = obj->m_name;
        m_loops = obj->m_loops;
        m_cell = obj->m_cell;
    }

    std::string Anim::GetName(){ return m_name; }

    std::string Anim::GetErrorText(){ return m_errorText; }

    PARSE_RESULT Anim::ParseXML(const TiXmlNode *dataNode)
    {
        const TiXmlElement* nodeElm = dataNode->ToElement();

        m_name = nodeElm->Attribute("name");
        if (m_name.empty())
        {
            m_errorText = "Cannot find attribute 'name' or the value is empty!";
            return PARSE_RESULT::ERROR_NAME_WRONG;
        }

        if (!nodeElm->Attribute("loops", (int*)&m_loops))
        {
            m_errorText = "Cannot find attribute 'loops' or the value is not numeric!";
            return PARSE_RESULT::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }

        const TiXmlNode *node = dataNode->FirstChild();
        while (node)
        {
            if (strcmp(node->Value(), "cell") == 0)
            {
                std::shared_ptr<Cell> cell = std::make_shared<Cell>();

                PARSE_RESULT result = cell->ParseXML(node);
                if (result == PARSE_RESULT::OK)
                    this->m_cell.push_back(cell);
                else
                {
                    m_errorText = "Parsing <cell> from <anim name='" + m_name + "'> Failed! >> " + cell->GetErrorText();
                    return result;
                }
            }
           
            node = node->NextSibling();
        }

        return PARSE_RESULT::OK;
    }

    void Anim::Update(uint64_t timestamp)
    {
        if (m_timestampLastChange == 0)
        {
            m_timestampLastChange = timestamp;
            m_currentCellIndex = 0;
            return;
        }

        int64_t diff = timestamp - m_timestampLastChange;

        if (diff <= 0)
            return;

        int32_t diffLocal = diff;

        size_t maxCells = m_cell.size();
        int32_t delay = m_cell[m_currentCellIndex]->GetDelay();
        while (diffLocal > delay)
        {
            diffLocal -= delay;

            m_currentCellIndex++;
            if (m_currentCellIndex == maxCells)
                m_currentCellIndex = 0;

            delay = (int64_t)m_cell[m_currentCellIndex]->GetDelay();
        }

        if (diffLocal != diff)
            m_timestampLastChange = timestamp;
    }

    std::shared_ptr<Cell> Anim::GetCurrentCell()
    {
        if (m_cell.empty())
            return nullptr;

        return m_cell[m_currentCellIndex];
    }







    Cell::Cell() : m_errorText(""), m_index(0), m_delay(0)
    {}

    std::uint32_t Cell::GetIndex(){ return m_index; }

    std::uint32_t Cell::GetDelay(){ return m_delay; }

    std::string Cell::GetErrorText(){ return m_errorText; }

    PARSE_RESULT Cell::ParseXML(const TiXmlNode *dataNode)
    {
        const TiXmlElement* nodeElm = dataNode->ToElement();

        if (!nodeElm->Attribute("index", (int*)&m_index))
        {
            m_errorText = "Cannot find attribute 'index' or the value is not numeric!";
            return PARSE_RESULT::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }

        if (!nodeElm->Attribute("delay", (int*)&m_delay))
        {
            m_errorText = "Cannot find attribute 'delay' or the value is not numeric!";
            return PARSE_RESULT::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }

        m_delay *= 100;

        const TiXmlNode *node = dataNode->FirstChild();
        while (node)
        {
            if (strcmp(node->Value(), "spr") == 0)
            {
                std::shared_ptr<CellSpr> cellspr = std::make_shared<CellSpr>();

                PARSE_RESULT result = cellspr->ParseXML(node);
                if (result == PARSE_RESULT::OK)
                    this->m_cellsSpr.push_back(cellspr);
                else
                {
                    m_errorText = "Parsing <spr> from <cell index=\"not_set\"> Failed! >> " + cellspr->GetErrorText();
                    return result;
                }
            }

            node = node->NextSibling();
        }

        return PARSE_RESULT::OK;
    }


    const std::vector< std::shared_ptr<CellSpr> >& Cell::GetCellsSpr()
    {
        return m_cellsSpr;
    }




    CellSpr::CellSpr() : m_errorText(""), m_name(""), m_x(0), m_y(0), m_z(0)
    {}


    std::string CellSpr::GetName(){ return m_name; }

    std::uint32_t CellSpr::GetX(){ return m_x; }

    std::uint32_t CellSpr::GetY(){ return m_y; }

    std::uint32_t CellSpr::GetZ(){ return m_z; }

    std::string CellSpr::GetErrorText(){ return m_errorText; }

    PARSE_RESULT CellSpr::ParseXML(const TiXmlNode *dataNode)
    {
        const TiXmlElement* nodeElm = dataNode->ToElement();

        m_name = nodeElm->Attribute("name");
        if (m_name.empty())
        {
            m_errorText = "Cannot find attribute 'name' or the value is empty!";
            return PARSE_RESULT::ERROR_NAME_WRONG;
        }

        if (!nodeElm->Attribute("x", (int*)&m_x))
        {
            m_errorText = "Cannot find attribute 'x' or the value is not numeric!";
            return PARSE_RESULT::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }

        if (!nodeElm->Attribute("y", (int*)&m_y))
        {
            m_errorText = "Cannot find attribute 'y' or the value is not numeric!";
            return PARSE_RESULT::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }

        if (!nodeElm->Attribute("z", (int*)&m_z))
        {
            m_errorText = "Cannot find attribute 'z' or the value is not numeric!";
            return PARSE_RESULT::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }

        return PARSE_RESULT::OK;
    }



} //namespace dfp