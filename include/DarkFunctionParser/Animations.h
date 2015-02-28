#ifndef DFP_ANIMATIONS_H
#define DFP_ANIMATIONS_H

#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <memory>

#include "../../src/Commons.h"

class TiXmlNode;

namespace dfp
{
    class Anim;
    class Cell;
    class CellSpr;

    

    /** This class is designed to read XML files with format like this:
    *------------------------------------------------------------
    * < ? xml version = "1.0" ? >
    * <!--Generated by darkFunction Editor(www.darkfunction.com)-->
    * <animations spriteSheet = "n69yj7.sprites" ver = "1.2">
    *   <anim name = "Animation" loops = "0">
    *       <cell index = "0" delay = "4">
    *           <spr name = "/broun/2" x = "0" y = "0" z = "0" / >
    *       </cell>
    *       <cell index = "1" delay = "4">
    *           <spr name = "/broun/10" x = "0" y = "0" z = "0" / >
    *       </cell>
    *   </anim>
    * </animations>
    *------------------------------------------------------------*/
    class Animations
    {
    public:

        /** The (default) constructor */
        Animations();

        /** Use it to get the filename (&path) of the sprite associated with this
        * animations. The path is relative to the *.anim file location.
        * @param onlyFileName used to specify if we want only the filename, or the filename withthe path.
        * @return a string with path and filename of the sprite.*/
        std::string GetSpriteFileName(bool onlyFileName = false);

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Getter for an Anim. This will return a new shared pointer.
        * Do not use this too often. I will create a new instance each time
        * you call it. Also the animations are stored in a map which is not so 
        * fast => So use it only to load the animations, not to draw them.
        * @param animName is the animation name.
        * @return an shared pointer to an Anim, OR a null shared pointer*/
        std::shared_ptr<Anim> GetAnim(const std::string& animName);

        /** Read a file and parse it.
        * Note: use '/' instead of '\\' as it is using '/' to find the path.
        * @param fileName is the filename and path of the sprite file (xml format).
        * @return PARSE_RESULT::OK if everithing was fine, or an error code!*/
        PARSE_RESULT ParseFile(const std::string &fileName);

        /** Parse text containing sprite formatted XML.
        * @param text is the xml sprite formatted.
        * @return PARSE_RESULT::OK if everithing was fine, or an error code!*/
        PARSE_RESULT ParseText(const std::string &text);

    private:

        /** Is the text for latest error */
        std::string m_errorText;
        
        /** This will store only the path to the Animation file. We need this path
        * to compose the path for sprite file, in function GetSpriteFileName*/
        std::string m_animationPath;

        /** The filename and path to the sprite associated with this animations*/
        std::string m_spriteFileName;

        /** is the version number from the xml node <animations spriteSheet="n69yj7.sprites" ver="1.2">*/
        std::string m_ver;

        /** This contains all the childs <anim> nodes. Is a map
        * of pair (Anim name, Anim instance)*/
        std::map< std::string, std::shared_ptr<Anim> > m_anim;

    };



    /** This is the class for <anim> node.
    * Ex:
    *   <anim name = "Animation" loops = "0">
    *       <cell index = "0" delay = "4">
    *           <spr name = "/broun/2" x = "0" y = "0" z = "0" / >
    *       </cell>
    *       <cell index = "1" delay = "4">
    *           <spr name = "/broun/10" x = "0" y = "0" z = "0" / >
    *       </cell>
    *   </anim>*/
    class Anim
    {
    public:

        /** The constructor */
        Anim();

        Anim(const std::shared_ptr<Anim> obj);

        Anim(const Anim &obj);

        /** Getter for the name of the node. */
        std::string GetName();

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <anim name = "Animation" loops = "0"> .
        * @return PARSE_RESULT::OK if everithing was fine, or an error code! */
        PARSE_RESULT ParseXML(const TiXmlNode *dataNode);


        /**
        * Update.  All the "update logic" must be placed in this function.
        * @param timestamp is the current time. */
        void Update(uint64_t timestamp);


        /** Use this to get the current cell, aka the frame of animation. 
        * The return value is changed by the Update function.
        * @return a shared pointerto an Cell instance.*/
        std::shared_ptr<Cell> GetCurrentCell();
        

    protected:

        /** Is the text for latest error */
        std::string m_errorText;

        /** Is the name of the <anim> */
        std::string m_name;

        /** The attribute loops from node <anim>*/
        int m_loops;

        /** This contains all the childs <cell> nodes.*/
        std::vector< std::shared_ptr<Cell> > m_cell;

        /** This is the current cell that must to be displayed */
        uint32_t m_currentCellIndex;

        /** This is the latest time when the m_currentCellIndex was changed. */
        uint64_t m_timestampLastChange;
    };




    /** This is the class for <cell> node.
    * Ex:
    *       <cell index = "0" delay = "4">
    *           <spr name = "/broun/2" x = "0" y = "0" z = "0" / >
    *       </cell>*/
    class Cell
    {
    public:

        /** The constructor */
        Cell();

        /** Getter for the Index */
        std::uint32_t GetIndex();

        /** Getter for the Delay */
        std::uint32_t GetDelay();

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <cell index = "0" delay = "4"> .
        * @return PARSE_RESULT::OK if everithing was fine, or an error code! */
        PARSE_RESULT ParseXML(const TiXmlNode *dataNode);

        /** Getter for the vector with all cellspr from a cell. 
        * @return a reference to the vector with CellSpr shared pointers. */
        const std::vector< std::shared_ptr<CellSpr> >& GetCellsSpr();

    protected:

        /** Is the text for latest error */
        std::string m_errorText;

        /** The index */
        std::uint32_t m_index;

        /** The delay */
        std::uint32_t m_delay;

        /** This contains all the childs <cell> nodes.*/
        std::vector< std::shared_ptr<CellSpr> > m_cellsSpr;

    };


    /** This is the class for <spr> node.
    * Ex:
    *   <spr name = "/broun/2" x = "0" y = "0" z = "0" / >*/
    class CellSpr
    {
    public:

        /** The constructor*/
        CellSpr();

        /** Getter for the name of the node. */
        std::string GetName();

        /** Getter for the sprite x */
        std::uint32_t GetX();

        /** Getter for the sprite y */
        std::uint32_t GetY();

        /** Getter for the sprite z */
        std::uint32_t GetZ();

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <spr name = "/broun/2" x = "0" y = "0" z = "0" / > .
        * @return PARSE_RESULT::OK if everithing was fine, or an error code! */
        PARSE_RESULT ParseXML(const TiXmlNode *dataNode);

    protected:

        /** Is the text for latest error */
        std::string m_errorText;

        /** Is the name of the <anim> */
        std::string m_name;

        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;

    };


}; //namespace dfp

#endif //DFP_H
