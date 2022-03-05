#include "test-assimp.h"
#include "main.h"

int DoTheImportThing( const char* pFile)
{
    // Start the import on the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll t
    // probably to request more postprocessing than we do in this example.
    const char* sphere = "s 0 0 0 10";
    const struct aiScene* scene = aiImportFileFromMemory(sphere,
                                                         strlen(sphere),
                                                         0, ".nff");
    // If the import failed, report it
    if(!scene)
    {
        // DoTheErrorLogging( aiGetErrorString());
        LOGE("Assimp import failed.");
        return false;
    }
    // Now we can access the file's contents
    // DoTheSceneProcessing( scene);
    // We're done. Release all resources associated with this import
    aiReleaseImport( scene);
    return true;
}
