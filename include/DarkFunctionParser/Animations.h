#ifndef DFP_ANIMATIONS_H
#define DFP_ANIMATIONS_H

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
    class Anim;
    class Cell;
    class CellSpr;

    

    /** This class is designed to read XML files generated
	* by DarkFunction editor (http://darkfunction.com/editor/)
	* with format like this:
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

		/** The "copy constructor" that takes a shared_ptr as input */
		Animations(const std::shared_ptr<Animations> obj);

		/** The copy constructor */
		Animations(const Animations &obj);

		~Animations();

		/** The asignment operator */
		Animations& operator=(const Animations& obj);


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
        * @return ParseResult::OK if everithing was fine, or an error code!*/
        ParseResult ParseFile(const std::string &fileName);

        /** Parse text containing sprite formatted XML.
        * @param text is the xml sprite formatted.
        * @return ParseResult::OK if everithing was fine, or an error code!*/
        ParseResult ParseText(const std::string &text);


		std::map< std::string, std::shared_ptr<Anim> >& GetAnims();


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

		/** The "copy constructor" that takes a shared_ptr as input */
        Anim(const std::shared_ptr<Anim> obj);

		/** The copy constructor */
        Anim(const Anim &obj);

		~Anim();

		/** The asignment operator */
		Anim& operator=(const Anim& other);

        /** Getter for the name of the node. */
        std::string GetName();

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <anim name = "Animation" loops = "0"> .
        * @return ParseResult::OK if everithing was fine, or an error code! */
        ParseResult ParseXML(const tinyxml2::XMLNode *dataNode);


        /**
        * Update.  All the "update logic" must be placed in this function.
        * @param dtSeconds time diference from last call (in seconds) 
		* @param animSpeedFactor is a factor that will accelerate or slow-down the animation.
		*        Ex: if animSpeedFactor=2.0 the standard delay between animation-frames will be
		*        delay / 2.0. */
		void Update(float dtSeconds, float animSpeedFactor);


        /** Use this to get the current cell, aka the frame of animation. 
        * The return value is changed by the Update function.
        * @return a shared pointerto an Cell instance.*/
		std::shared_ptr<Cell> GetCurrentCell();
        
		std::vector< std::shared_ptr<Cell> >& GetCells();

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

        /** This is the time elapsed from the last Update call */
        float m_timestampLastChange;
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

		~Cell();

        /** Getter for the Index */
        unsigned int GetIndex();

        /** Getter for the Delay */
        unsigned int GetDelay();

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <cell index = "0" delay = "4"> .
        * @return ParseResult::OK if everithing was fine, or an error code! */
        ParseResult ParseXML(const tinyxml2::XMLNode *dataNode);

        /** Getter for the vector with all cellspr from a cell. 
        * @return a reference to the vector with CellSpr shared pointers. */
        const std::vector< std::shared_ptr<CellSpr> >& GetCellsSpr();

    protected:

        /** Is the text for latest error */
        std::string m_errorText;

        /** The index */
        unsigned int m_index;

        /** The delay */
        unsigned int m_delay;

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

		~CellSpr();

        /** Getter for the name of the node. */
        std::string GetName();

        /** Getter for the sprite x */
        unsigned int GetX();

        /** Getter for the sprite y */
        unsigned int GetY();

        /** Getter for the sprite z */
        unsigned int GetZ();

        /** Get the text for latest error!
        * @return a string with a text that describe the error.*/
        std::string GetErrorText();

        /** Parse text containing spr XML node.
        * @param dataNode is the xml node for <spr name = "/broun/2" x = "0" y = "0" z = "0" / > .
        * @return ParseResult::OK if everithing was fine, or an error code! */
        ParseResult ParseXML(const tinyxml2::XMLNode *dataNode);

    protected:

        /** Is the text for latest error */
        std::string m_errorText;

        /** Is the name of the <anim> */
        std::string m_name;

        unsigned int m_x;
        unsigned int m_y;
        unsigned int m_z;

    };


}; //namespace dfp

#endif //DFP_H

