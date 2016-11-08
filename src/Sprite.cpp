#include "DarkFunctionParser/Sprite.h"

#if defined(I3D_PLATFORM_S3E)
#include "tinyxml.h"
#else
#include <tinyxml2/tinyxml2.h>
#endif


#include <sstream>

namespace dfp
{
    Sprite::Sprite() 
        : m_errorText("")
        , m_imagePath("")
        , m_imageFileName("")
        , m_imageW(0)
        , m_imageH(0)
    {}



    std::string Sprite::GetImageFileName(bool onlyFileName)
    { 
        if (onlyFileName)
            return m_imageFileName;

        std::string imagePathFileName = "";
        if (!m_imagePath.empty())
            imagePathFileName = m_imagePath;
        if (*m_imagePath.rbegin() != '/')
            imagePathFileName += "/";

        imagePathFileName += m_imageFileName;

        return imagePathFileName;
    }

    std::string Sprite::GetErrorText(){ return m_errorText; }

    ParseResult Sprite::ParseFile(const std::string &fileName)
    {
        ParseResult errorsCode = ParseResult::OK;

        int lastSlash = fileName.find_last_of("/");

        // Get the directory of the file using substring.
        if (lastSlash > 0)
        {
            m_imagePath = fileName.substr(0, lastSlash + 1);
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
            errorsCode = ParseResult::ERROR_COULDNT_OPEN;
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
            errorsCode = ParseResult::ERROR_INVALID_FILE_SIZE;
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

    ParseResult Sprite::ParseText(const std::string &text)
    {
        // Create a tiny xml document and use it to parse the text.
		tinyxml2::XMLDocument doc;
        doc.Parse(text.c_str());

        // Check for parsing errors.
        if (doc.Error())
        {            
            m_errorText = doc.GetErrorStr1();
			m_errorText += doc.GetErrorStr2();
            return ParseResult::ERROR_PARSING_FAILED;
        }

		tinyxml2::XMLNode *imgNode = doc.FirstChildElement("img");
        if (!imgNode)
        {
            m_errorText = "Cannot find node <img> !";
            return ParseResult::ERROR_MISSING_NODE;
        }

		tinyxml2::XMLElement* imgElem = imgNode->ToElement();

        // Read the map attributes.
        m_imageFileName = imgElem->Attribute("name");
        if (m_imageFileName.empty())
        {
            m_errorText = "Cannot find attribute 'name' or the value is empty!";
            return ParseResult::ERROR_IMAGE_PATHNAME_WRONG;
        }

		const char* tempValue = imgElem->Attribute("w");
        if (!tempValue)
        {
            m_errorText = "Cannot find attribute 'w' or the value is not numeric!";
            return ParseResult::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }
		m_imageW = std::stoi(tempValue);

		tempValue = imgElem->Attribute("h");
		if (!tempValue)
        {
            m_errorText = "Cannot find attribute 'h' or the value is not numeric!";
            return ParseResult::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }
		m_imageH = std::stoi(tempValue);

        const tinyxml2::XMLNode *node = imgElem->FirstChild();

        if (!node)
        {
            m_errorText = "The <img> node does not have child nodes!";
            return ParseResult::ERROR_IMAGE_PATHNAME_WRONG;
        }

        while (node)
        {
            // Read the map properties.
            if (strcmp(node->Value(), "definitions") == 0)
            {
                const tinyxml2::XMLNode *subnode = node->FirstChild();
                while (subnode)
                {
                    if (strcmp(subnode->Value(), "dir") == 0)
                    {
                        std::shared_ptr<Dir> dir = std::make_shared<Dir>();

                        ParseResult result = dir->ParseXML(subnode);
                        if (result == ParseResult::OK)
                        {
                            m_root = dir;
                            if (m_root->GetName().compare("/") != 0)
                            {
                                m_errorText = "The root <dir> is missing!";
                                return ParseResult::ERROR_ROOT_MISSING;
                            }
                        }
                        else
                        {
                            m_errorText = "Parsing <dir> Failed! >> " + dir->GetErrorText();
                            return result;
                        }
                    }

                    subnode = subnode->NextSibling();
                }

            }
            
            node = node->NextSibling();
        }

        return ParseResult::OK;
    }

    std::shared_ptr<Spr> Sprite::GetSpr(const std::string& xmlPath)
    {
        if (xmlPath.empty())
            return nullptr;

        if (xmlPath.at(0) != '/')
            return nullptr;

        if (m_root)
            return m_root->GetSpr(xmlPath.substr(1));
        
        return nullptr;
    }

	std::vector<std::shared_ptr<Spr> > Sprite::GetAllSpr()
	{
		std::vector<std::shared_ptr<Spr> > results;

		std::vector<std::shared_ptr<Spr> > r = GetAllSpr(m_root);
		for (auto s : r)
		{
			results.push_back(s);
		}

		for (auto sitem : m_root->m_spr)
		{
			std::shared_ptr<Spr> s = sitem.second;
			results.push_back(s);
		}
		
		return results;
	}

	std::vector<std::shared_ptr<Spr> > Sprite::GetAllSpr(std::shared_ptr<Dir> dir)
	{
		std::vector<std::shared_ptr<Spr> > results;
		for (auto ditem : dir->m_dir)
		{
			std::shared_ptr<Dir> d = ditem.second;

			std::vector<std::shared_ptr<Spr> > r = GetAllSpr(d);
			for (auto s : r)
			{
				results.push_back(s);
			}

			for (auto sitem : d->m_spr)
			{
				std::shared_ptr<Spr> s = sitem.second;
				results.push_back(s);
			}
		}

		return results;
	}
    



    Spr::Spr( std::string name, unsigned int x, unsigned int y, unsigned int w, unsigned int h) 
        : m_errorText(""), m_name(name)
        , m_x(x), m_y(y), m_w(w), m_h(h)
    {}

    std::string Spr::GetName(){ return m_name; }

    unsigned int Spr::GetX(){ return m_x; }

    unsigned int Spr::GetY(){ return m_y; }

    unsigned int Spr::GetW(){ return m_w; }

    unsigned int Spr::GetH(){ return m_h; }

    std::string Spr::GetErrorText(){ return m_errorText; }

    ParseResult Spr::ParseXML(const tinyxml2::XMLNode *dataNode)
    {
        const tinyxml2::XMLElement* nodeElm = dataNode->ToElement();

        m_name = nodeElm->Attribute("name");
        if (m_name.empty())
        {
            m_errorText = "Cannot find attribute 'name' or the value is empty!";
            return ParseResult::ERROR_NAME_WRONG;
        }

		const char* tempValue = nodeElm->Attribute("x");
		if (!tempValue)
        {
            m_errorText = "Cannot find attribute 'x' or the value is not numeric!";
            return ParseResult::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }
		m_x = std::stoi(tempValue);

		tempValue = nodeElm->Attribute("y");
		if (!tempValue)
        {
            m_errorText = "Cannot find attribute 'y' or the value is not numeric!";
            return ParseResult::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }
		m_y = std::stoi(tempValue);

		tempValue = nodeElm->Attribute("w");
		if (!tempValue)
        {
            m_errorText = "Cannot find attribute 'w' or the value is not numeric!";
            return ParseResult::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }
		m_w = std::stoi(tempValue);

		tempValue = nodeElm->Attribute("h");
		if (!tempValue)
        {
            m_errorText = "Cannot find attribute 'h' or the value is not numeric!";
            return ParseResult::ERROR_NUMERIC_ATTRIBUTE_WRONG;
        }
		m_h = std::stoi(tempValue);

        return ParseResult::OK;
    }





    Dir::Dir() : m_errorText(""), m_name("")
    {}

    std::string Dir::GetName(){ return m_name; }

    std::string Dir::GetErrorText(){ return m_errorText; }

    ParseResult Dir::ParseXML(const tinyxml2::XMLNode *dataNode)
    {
        const tinyxml2::XMLElement* nodeElm = dataNode->ToElement();

        m_name = nodeElm->Attribute("name");
        if (m_name.empty())
        {
            m_errorText = "Cannot find attribute 'name' or the value is empty!";
            return ParseResult::ERROR_NAME_WRONG;
        }

        const tinyxml2::XMLNode *node = dataNode->FirstChild();
        while (node)
        {
            if (strcmp(node->Value(), "dir") == 0)
            {
                std::shared_ptr<Dir> dir = std::make_shared<Dir>();

                ParseResult result = dir->ParseXML(node);
                if (result == ParseResult::OK)
                    this->m_dir[dir->GetName()] = dir;
                else
                {
                    m_errorText = "Parsing <dir> from <dir name='" + m_name + "'> Failed! >> " + dir->GetErrorText();
                    return result;
                }
            }
            else if (strcmp(node->Value(), "spr") == 0)
            {
                std::shared_ptr<Spr> spr = std::make_shared<Spr>();

                ParseResult result = spr->ParseXML(node);
                if (result == ParseResult::OK)
                    this->m_spr[spr->GetName()] = spr;
                else
                {
                    m_errorText = "Parsing <spr> from <dir name='" + m_name + "'> Failed! >> " + spr->GetErrorText();
                    return result;
                }
            }

            node = node->NextSibling();
        }

        return ParseResult::OK;
    }

    std::shared_ptr<Spr> Dir::GetSpr(const std::string& xmlPath)
    {
        if (xmlPath.empty())
            return nullptr;

        size_t pos = xmlPath.find("/");

        if (pos == std::string::npos)
        {
            /// This must be the end of the path, so must be a sprite
            return m_spr[xmlPath];
        }

        std::string dirName = xmlPath.substr(0, pos);

        if (dirName.empty())
            return nullptr;

        auto it = m_dir.find(dirName);

        if (it == m_dir.end())
            return nullptr;

        std::shared_ptr<Dir> dir = m_dir[dirName];
        if (!dir)
            return nullptr;

        return dir->GetSpr(xmlPath.substr(pos + 1));
    }






} //namespace dfp