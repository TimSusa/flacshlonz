#ifndef STATES_HPP
#define STATES_HPP

namespace states
{
    enum eMainState
    {
        MAINSTATE_NONE = 0,
        MAINSTATE_IDLE,
        MAINSTATE_DB_IMPORT,
        MAINSTATE_FILE_SCAN
    };

    enum eButtonState
    {
        BUTTON_NONE = 0,
        BUTTON_IDLE,
        BUTTON_SET_PATH,
        BUTTON_UPDATE,
        BUTTON_STOP
    };

    enum eFileSystemManagerState
    {
        FM_NONE = 0,
        FM_IDLE,
        FM_SCANNING_MUSIC_COLLECTION,
        FM_SCANNING_MUSIC_COLLECTION_FINISHED,
        FM_SCANNING_FILE_INCOMING,
        FM_SCANNING_FILE_INCOMING_FINISHED,
        FM_DB_IMPORT_COLLECTION,
        FM_DB_IMPORT_COLLECTION_FINISHED,
        FM_DB_IMPORT_INCOMING,
        FM_DB_IMPORT_INCOMING_FINISHED,
        FM_COPY,
        FM_REMOVE
    };
}

#endif // STATES_HPP
