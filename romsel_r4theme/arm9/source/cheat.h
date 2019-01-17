
#ifndef CHEAT_H
#define CHEAT_H

#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <stdint.h>
#include <nds.h>

#define CHEAT_CODE_END	0xCF000000
#define CHEAT_ENGINE_RELOCATE	0xCF000001
#define CHEAT_ENGINE_HOOK	0xCF000002


typedef unsigned int CheatWord;

class CheatFolder;

class CheatBase 
{
private:
	CheatFolder* parent;
	
public:
	std::string name;
	std::string note;
	
	CheatBase (CheatFolder* parent)
	{
		this->parent = parent;
	}

	CheatBase (const char* name, CheatFolder* parent)
	{
		this->name = name;
		this->parent = parent;
	}
	
	CheatBase (const std::string name, CheatFolder* parent)
	{
		this->name = name;
		this->parent = parent;
	}
	
	virtual ~CheatBase () 
	{
	}
	
	const char* getName (void)
	{
		return name.c_str();
	}
	
	const char* getNote (void)
	{
		return note.c_str();
	}
	
	CheatFolder* getParent (void)
	{
		return parent;
	}
	
	virtual std::list<CheatWord> getEnabledCodeData(void)
	{
		std::list<CheatWord> codeData;
		return codeData;
	}
} ;

class CheatCode : public CheatBase 
{
public:
	CheatCode (CheatFolder* parent) : CheatBase (parent)
	{
		enabled = false;
		always_on = false;
		master = false;
	}

	void setEnabled (bool enable) 
	{
		if (!always_on) {
			enabled = enable;
		}
	}
	
	void toggleEnabled (void);
	
	bool getEnabledStatus (void) 
	{
		return enabled;
	}
	
	bool isMaster (void)
	{
		return master;
	}
	
	std::list<CheatWord> getCodeData(void)
	{
		return cheatData;
	}
	
	void setCodeData (const std::string& data);
	
	std::list<CheatWord> getEnabledCodeData(void);
	
	static const int CODE_WORD_LEN = 8;
	
private:
	std::list<CheatWord> cheatData;
	bool enabled;
	bool always_on;
	bool master;
} ;

class CheatFolder : public CheatBase
{
public:
	CheatFolder (const char* name, CheatFolder* parent) : CheatBase (name, parent)
	{
		allowOneOnly = false;
	}
	
	CheatFolder (CheatFolder* parent) : CheatBase (parent)
	{
		allowOneOnly = false;
	}

	~CheatFolder();
	
	void addItem (CheatBase* item)
	{
		if (item) {
			contents.push_back(item);
		}
	}
	
	void enablingSubCode (void);

	void enableAll (bool enabled);
	
	void setAllowOneOnly (bool value) 
	{
		allowOneOnly = value;
	}
	
	std::vector<CheatBase*> getContents(void) 
	{
		return contents;
	}
	
	std::list<CheatWord> getEnabledCodeData(void);
	
protected:
	std::vector<CheatBase*> contents;

private:
	bool allowOneOnly;
	
} ;

class CheatGame : public CheatFolder
{
public:
	CheatGame (const char* name, CheatFolder* parent) : CheatFolder (name, parent)
	{
		memset(gameid, ' ', 4);
	}
	
	CheatGame (CheatFolder* parent) : CheatFolder (parent)
	{
		memset(gameid, ' ', 4);
	}
	
	bool checkGameid (const char gameid[4], uint32_t headerCRC)
	{
        #ifdef DEBUG
        nocashMessage("CheatGame::checkGameid");
        
        
        char* gameIdS;
        sprintf (gameIdS, "%c%c%c%c", gameid[0], gameid[1], gameid[2], gameid[3]);
        nocashMessage("gameId");
        nocashMessage(gameIdS);
        
        sprintf (gameIdS, "%c%c%c%c", this->gameid[0], this->gameid[1], this->gameid[2], this->gameid[3]);
        nocashMessage("this->gameId");
        nocashMessage(gameIdS);
        
        char * headerCrcS;
        sprintf (headerCrcS, "%08X", headerCRC);                        
        nocashMessage("headerCRC");
        nocashMessage(headerCrcS);
        
        sprintf (headerCrcS, "%08X",  this->headerCRC);                        
        nocashMessage("this->headerCRC");
        nocashMessage(headerCrcS);
        #endif
        
		return (memcmp (gameid, this->gameid, sizeof(this->gameid)) == 0) &&
			(headerCRC == this->headerCRC);
	}

	void setGameid (const std::string& id);

private:
	char gameid[4];
	uint32_t  headerCRC;
} ;

class CheatCodelist : public CheatFolder 
{
public:
	CheatCodelist (void) : CheatFolder ("No codes loaded", NULL)
	{
	}
	
	~CheatCodelist ();
	
	bool load (FILE* fp, const char gameid[4], uint32_t headerCRC, bool filter);

	CheatGame* getGame (const char gameid[4], uint32_t headerCRC);

	bool parse(const std::string& aFileName);

	bool searchCheatData(FILE* aDat,u32 gamecode,u32 crc32,long& aPos,size_t& aSize);

	bool parseInternal(FILE* aDat,u32 gamecode,u32 crc32);

	void generateList(void);

	bool romData(const std::string& aFileName,u32& aGameCode,u32& aCrc32);

	private:
    struct sDatIndex
    {
      u32 _gameCode;
      u32 _crc32;
      u64 _offset;
    };
    class cParsedItem
    {
      public:
        std::string _title;
        std::string _comment;
        std::string _cheat;
        u32 _flags;
        u32 _offset;
        cParsedItem(const std::string& title,const std::string& comment,u32 flags,u32 offset=0):_title(title),_comment(comment),_flags(flags),_offset(offset) {};
        enum
        {
          EFolder=1,
          EInFolder=2,
          EOne=4,
          ESelected=8,
          EOpen=16
        };
    };
    enum
    {
      EIconColumn=0,
      ETextColumn=1,
      EIconWidth=15,
      EFolderWidth=11,
      ERowHeight=15,
      EFolderTop=3,
      ESelectTop=5
    };
  private:
    std::vector<cParsedItem> _data;
    std::vector<size_t> _indexes;
    std::string _fileName;
  public:
    std::string getCheats();
	
private:
	enum TOKEN_TYPE {TOKEN_DATA, TOKEN_TAG_START, TOKEN_TAG_END, TOKEN_TAG_SINGLE};

	std::string nextToken (FILE* fp, TOKEN_TYPE& tokenType);

} ;

#endif // CHEAT_H

