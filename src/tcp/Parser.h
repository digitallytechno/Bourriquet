/***************************************************************************
 Bourriquet MorphOS
 2024-2025 digitally
 
***************************************************************************/
#ifndef PARSER_H
#define PARSER_H

// client <-> server
#define OP_LOGINREQUEST			0x01	//<HASH 16><ID 4><PORT 2><1 Tag_set>
#define OP_REJECT				0x05	//(null)
#define OP_GETSERVERLIST		0x14	//(null)client->server
#define	OP_OFFERFILES			0x15	// <count 4>(<HASH 16><ID 4><PORT 2><1 Tag_set>)[count]
#define OP_SEARCHREQUEST		0x16	// <Query_Tree>
#define OP_DISCONNECT			0x18	// (not verified)
#define OP_GETSOURCES			0x19	// <HASH 16>
#define	OP_SEARCH_USER			0x1A	// <Query_Tree>
#define OP_CALLBACKREQUEST		0x1C	// <ID 4>
#define OP_QUERY_MORE_RESULT    0x21    // (null)
#define OP_SERVERLIST			0x32	// <count 1>(<IP 4><PORT 2>)[count] server->client
#define OP_SEARCHRESULT			0x33	// <count 4>(<HASH 16><ID 4><PORT 2><1 Tag_set>)[count]
#define	OP_SERVERSTATUS			0x34	// <USER 4><FILES 4>
#define OP_CALLBACKREQUESTED	0x35	// <IP 4><PORT 2>
#define OP_CALLBACK_FAIL		0x36	// (null not verified)
#define OP_SERVERMESSAGE		0x38	// <len 2><Message len>
#define OP_IDCHANGE				0x40	// <NEW_ID 4><server_flags 4><primary_tcp_port 4 (unused)><client_IP_address 4>
#define OP_SERVERIDENT		    0x41	// <HASH 16><IP 4><PORT 2>{1 TAG_SET}
#define OP_FOUNDSOURCES			0x42	// <HASH 16><count 1>(<ID 4><PORT 2>)[count]
#define OP_USERS_LIST           0x43    // <count 4>(<HASH 16><ID 4><PORT 2><1 Tag_set>)[count]
#define OP_GETSOURCES_OBFU		0x23
#define OP_FOUNDSOURCES_OBFU	0x44    // <HASH 16><count 1>(<ID 4><PORT 2><obf settings 1>(UserHash16 if obf&0x08))[count]

// client <-> client
//#define	OP_HELLO				0x01	// 0x10<HASH 16><ID 4><PORT 2><1 Tag_set>
#define OP_SENDINGPART			0x46	// <HASH 16><von 4><bis 4><Daten len:(von-bis)>
#define	OP_REQUESTPARTS			0x47	// <HASH 16><von[3] 4*3><bis[3] 4*3>
#define OP_FILEREQANSNOFIL		0x48	// <HASH 16>
#define OP_END_OF_DOWNLOAD		0x49    // <HASH 16>
#define OP_ASKSHAREDFILES		0x4A	// (null)
#define OP_ASKSHAREDFILESANSWER 0x4B	// <count 4>(<HASH 16><ID 4><PORT 2><1 Tag_set>)[count]
#define	OP_HELLOANSWER			0x4C	// <HASH 16><ID 4><PORT 2><1 Tag_set><SERVER_IP 4><SERVER_PORT 2>
#define OP_CHANGE_CLIENT_ID		0x4D	// <ID_old 4><ID_new 4>
#define	OP_MESSAGE				0x4E	// <len 2><Message len>
#define OP_SETREQFILEID			0x4F	// <HASH 16>
#define	OP_FILESTATUS			0x50	// <HASH 16><count 2><status(bit array) len:((count+7)/8)>
#define	OP_STARTUPLOADREQ		0x54	// <HASH 16>
#define	OP_ACCEPTUPLOADREQ		0x55	// (null)
#define	OP_CANCELTRANSFER		0x56	// (null)
#define OP_OUTOFPARTREQS		0x57	// (null)
#define OP_REQUESTFILENAME		0x58	// <HASH 16>	(more correctly file_name_request)
#define OP_REQFILENAMEANSWER	0x59	// <HASH 16><len 4><NAME len>
#define OP_CHANGE_SLOT			0x5B	// <HASH 16>
#define OP_QUEUERANK			0x5C	// <wert  4> (slot index of the request)
#define OP_ASKSHAREDDIRS        0x5D    // (null)
#define OP_ASKSHAREDFILESDIR    0x5E    // <len 2><Directory len>
#define OP_ASKSHAREDDIRSANS     0x5F    // <count 4>(<len 2><Directory len>)[count]
#define OP_ASKSHAREDFILESDIRANS 0x60    // <len 2><Directory len><count 4>(<HASH 16><ID 4><PORT 2><1 Tag_set>)[count]
#define OP_ASKSHAREDDENIEDANS   0x61    // (null)

// this 'identifier' is used for referencing shared part (incomplete) files with the OP_ASKSHAREDDIRS and related opcodes
// it was introduced with eDonkeyHybrid and is considered as part of the protocol.
#define OP_INCOMPLETE_SHARED_FILES "!Incomplete Files"
#define OP_OTHER_SHARED_FILES	   "!Other" // files which are not within a shared directory (single shared files)

void parse_cmd(struct Host *host, char *buf);
void parse_host_reply(struct Host *host, unsigned char *buf);
void uploadGapHandle(struct Host *dl, long start, long end);
int ParseEDKTags_Download(unsigned char *buf, struct DownloadList *p);
const unsigned char *hash_bin2hex(const unsigned char *hash);
const unsigned char *hash_hex2bin(const char hash_str[35]);

#endif /* PARSER_H */