#ifndef DFP_SPRITE_H
#define DFP_SPRITE_H

#include <string>
//#include <cstdint>
#include <vector>
#include <map>
#include <memory>


#include "Commons.h"


namespace tinyxml2
{
	class XMLNode;
};

namespace dfp
{
    class Spr;
    class Dir;

    /** This class is designed to read XML files generated
	* by DarkFunction editor (http://darkfunction.com/editor/)
	* with format like this:
    *------------------------------------------------------------
    * < ? xml version = "1.0" ? >
    * <img name = "n69yj7.bmp" w = "954" h = "1033">
    *    <definitions>
    *         <dir name = "/">
    *             <dir name = "brown">
    *                 <spr name = "0" x = "5" y = "7" w = "17" h = "24" / >
    *                 <spr name = "1" x = "38" y = "7" w = "14" h = "24" / >
    *             < / dir>
    *         < / dir>
    *     < / definitions>
    * < / img>
    * ------------------------------------------------------------*/
    class Sprite
    {
    public:

        /** The (default) constructor */
        Sprite();

        /** Use it to get the filename (&path) of the image 
        * associated with this sprite. The path is relative to the *.sprite file location.
        * @param onlyFileName used to specify if we want only the filename, or the filename withthe path.
        * @return a string with path and filename of the image.*/
        std::string GetImageFileName(bool onlyFileName = false);

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Read a file and parse it.
        * Note: use '/' instead of '\\' as it is using '/' to find the path.
        * @param fileName is the filename and path of the sprite file (xml format).
        * @return ParseResult::OK if everithing was fine, or an error code!*/
        ParseResult ParseFile(const std::string &fileName);

        /** Parse text containing sprite formatted XML.
        * @param text is the xml sprite formatted.
        * @return ParseResult::OK if everithing was fine, or an error code!*/
        ParseResult ParseText(const std::string &text);

        /** This will return a shared pointer to a sprite (Spr) found at location
        * described by the xmlPath. For example if the sprite file is:
        * ------------------------------------------------------------
        * <img name="n69yj7.bmp" w="954" h="1033">
        *    <definitions>
        *         <dir name="/">
        *             <dir name="brown">
        *                 <spr name="0" x="5" y="7" w="17" h="24"/>
        *                 <spr name="1" x="38" y="7" w="14" h="24"/>
        *             </dir>
        *         </dir>
        *     </definitions>
        * </img>
        * ------------------------------------------------------------
        * The xmlPath for sprite with name '0' is: '/brown/0' .
        *
        * @param xmlPath is the path to the sprite.
        * @return a shared pointer for a Sprite object.*/
		std::shared_ptr<Spr> GetSpr(const std::string& xmlPath);

    private:

        /** Is the text for latest error */
        std::string m_errorText;

        /** This will store only the path to the image file. We need this path
        * to compose the path for sprite file, in function GetImageFileName*/
        std::string m_imagePath;

        /** The filename and path to the image of this sprite*/
        std::string m_imageFileName;

        /** The width of the image*/
		unsigned int m_imageW;

        /** The height of the image*/
		unsigned int m_imageH;

        /** The object that contains in a tree format all the other <dir> notes from xml*/
        std::shared_ptr<Dir> m_root;
    };


    /** This is the class for <dir> node.
    * Ex: 
    *   <dir name="/">
    *       <dir name="brown">
    *           <spr name="0" x="5" y="7" w="17" h="24"/>
    *           <spr name="1" x="38" y="7" w="14" h="24"/>
    *       </dir>
    *   </dir>*/
    class Dir
    {
        friend class Spr;
    public:

        /** The constructor */
        Dir();

        /** Getter for the Dir name */
        std::string GetName();

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <dir name="brown"> .
        * @return ParseResult::OK if everithing was fine, or an error code! */
        ParseResult ParseXML(const tinyxml2::XMLNode *dataNode);

        /** This will return a shared pointer to a sprite (Spr) found at location
        * described by the xmlPath. For example if the sprite is file is:
        * ------------------------------------------------------------
        * <img name="n69yj7.bmp" w="954" h="1033">
        *    <definitions>
        *         <dir name="/">
        *             <dir name="brown">
        *                 <spr name="0" x="5" y="7" w="17" h="24"/>
        *                 <spr name="1" x="38" y="7" w="14" h="24"/>
        *             </dir>
        *         </dir>
        *     </definitions>
        * </img>
        * ------------------------------------------------------------
        * The xmlPath for sprite with name '0' is: '/brown/0' .
        *
        * @param xmlPath is the path to the sprite.
        * @return a shared pointer for a Sprite object.*/
        std::shared_ptr<Spr> GetSpr(const std::string& xmlPath);

    protected:

        /** Is the text for latest error */
        std::string m_errorText;

        /** The name of the Dir */
        std::string m_name;

        /** This contains all the childs <dir> nodes. Is a map 
        * of pair (Dir name, Dir instance)*/
        std::map< std::string, std::shared_ptr<Dir> > m_dir;

        /** This contains all the childs <spr> nodes  Is a map 
        * of pair (Spr name, Spr instance)*/
        std::map< std::string, std::shared_ptr<Spr> > m_spr;
    };



    /** This is the class for <spr> node.
    * Ex: <spr name="0" x="5" y="7" w="17" h="24"/> */
    class Spr
    {
    public:
        /** The constructor
        * @param name is the name of the sprite 
        * @param x is the position in the image.
        * @param y is the position in the image.
        * @param w is the width.
        * @param h is the height. */
        Spr(std::string name = "", 
			unsigned int x = 0, unsigned int y = 0,
			unsigned int w = 0, unsigned int h = 0);

        /** Getter for the sprite name */
        std::string GetName();

        /** Getter for the sprite x */
        unsigned int GetX();

        /** Getter for the sprite y */
		unsigned int GetY();

        /** Getter for the sprite w */
		unsigned int GetW();

        /** Getter for the sprite h */
		unsigned int GetH();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <spr name="0" x="5" y="7" w="17" h="24"/> .
        * @return ParseResult::OK if everithing was fine, or an error code! */
        ParseResult ParseXML(const tinyxml2::XMLNode *dataNode);

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

    protected:

        /** Is the text for latest error */
        std::string m_errorText;

        /** The name of the sprite */
        std::string m_name;

        int m_x;
        int m_y;
        int m_w;
        int m_h;
    };
}; //namespace dfp

#endif //DFP_SPRITE_H

