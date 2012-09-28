/*
 *
 *  The Sleuth Kit
 *
 *  Contact: Brian Carrier [carrier <at> sleuthkit [dot] org]
 *
 *  This is free and unencumbered software released into the public domain.
 *  
 *  Anyone is free to copy, modify, publish, use, compile, sell, or
 *  distribute this software, either in source code form or as a compiled
 *  binary, for any purpose, commercial or non-commercial, and by any
 *  means.
 *  
 *  In jurisdictions that recognize copyright laws, the author or authors
 *  of this software dedicate any and all copyright interest in the
 *  software to the public domain. We make this dedication for the benefit
 *  of the public at large and to the detriment of our heirs and
 *  successors. We intend this dedication to be an overt act of
 *  relinquishment in perpetuity of all present and future rights to this
 *  software under copyright law.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE. 
 */

/**
 * \file EntropyModule.cpp
 * Contains the implementation of a file analysis module that calculates the
 * entropy of a file's contents.
 *
 * This sample module shows a basic Sleuth Kit Framework module.  It is
 * released as public domain and you are free to remove this header,  use
 * it as a starting point for your module, and choose whatever license that
 * you want.  Note that the framework itself is NOT public domain.
 */

// TSK Framework includes
#include "TskModuleDev.h"

// Poco includes
// Uncomment this include if using Poco.
//#include "Poco/Exception.h"

// C/C++ library includes
#include <string>
#include <sstream>
#include <math.h>

// Functions and variables other than the module API functions are enclosed in
// an anonymous namespace to give them file scope instead of global scope. This
// replaces the older practice of declaring file scope functions and variables
// using the "static" keyword.
namespace
{
    const char *MODULE_NAME = "Entropy";
    const char *MODULE_DESCRIPTION = "Performs an entropy calculation for the contents of a given file";
    const char *MODULE_VERSION = "1.0.0";
    
    const uint32_t FILE_BUFFER_SIZE = 8193;

    double calculateEntropy(TskFile *pFile)
    {
        unsigned __int8 byte = 0;
        long byteCounts[256];
        memset(byteCounts, 0, sizeof(long) * 256);
        long totalBytes = 0;
        char buffer[FILE_BUFFER_SIZE];
        int bytesRead = 0;

        do
        {
            memset(buffer, 0, FILE_BUFFER_SIZE);
            bytesRead = pFile->read(buffer, FILE_BUFFER_SIZE);
            for(int i = 0; i < bytesRead; ++i)
            {
                byte = buffer[i];
                byteCounts[byte]++;
            }
            totalBytes += bytesRead;
        } 
        while (bytesRead > 0);

        double entropy = 0.0;
        for(int i = 0; i<256; ++i)
        {
            double p = static_cast<double>(byteCounts[i]) / static_cast<double>(totalBytes);
            if(p > 0.0)
            {
                entropy -= p *(log(p)/log(2.0));
            }
        }

        return entropy;
    }
}

extern "C" 
{
    /**
     * Module identification function. 
     *
     * @return The name of the module.
     */
    TSK_MODULE_EXPORT const char *name()
    {
        return MODULE_NAME;
    }

    /**
     * Module identification function. 
     *
     * @return A description of the module.
     */
    TSK_MODULE_EXPORT const char *description()
    {
        return MODULE_DESCRIPTION;
    }

    /**
     * Module identification function. 
     *
     * @return The version of the module.
     */
    TSK_MODULE_EXPORT const char *version()
    {
        return MODULE_VERSION;
    }

    /**
     * Module initialization function. Receives a string of initialization arguments, 
     * typically read by the caller from a pipeline configuration file. 
     * Returns TskModule::OK or TskModule::FAIL. Returning TskModule::FAIL indicates 
     * the module is not in an operational state.  
     *
     * @param args a string of initialization arguments.
     * @return TskModule::OK if initialization succeeded, otherwise TskModule::FAIL.
     */
    TskModule::Status TSK_MODULE_EXPORT initialize(const char* arguments)
    {    
        // The TSK Framework convention is to prefix error messages with the
        // name of the module/class and function that emitted the message. 
        std::ostringstream msgPrefix;
        msgPrefix << name() << "::initialize : ";

        // Well-behaved modules should catch and log all possible exceptions
        // and return an appropriate TskModule::Status to the TSK Framework. 
        TskModule::Status status = TskModule::OK;
        try
        {
            // If this module required initialization, the initialization code would
            // go here.
        }
        catch (TskException &ex)
        {
            status = TskModule::FAIL;
            std::ostringstream msg;
            msg << msgPrefix.str() << "TskException: " << ex.message();
            LOGERROR(msg.str());
        }
        // Uncomment this catch block and the #include of "Poco/Exception.h" if using
        // Poco.
        //catch (Poco::Exception &ex)
        //{
        //    status = TskModule::FAIL;
        //    std::ostringstream msg;
        //    msg << msgPrefix.str() << "Poco::Exception: " << ex.displayText();
        //    LOGERROR(msg.str());
        //}
        catch (std::exception &ex)
        {
            status = TskModule::FAIL;
            std::ostringstream msg;
            msg << msgPrefix.str() << "std::exception: " << ex.what();
            LOGERROR(msg.str());
        }
        catch (...)
        {
            status = TskModule::FAIL;
            LOGERROR(msgPrefix.str() + "unrecognized exception");
        }

        return status;
    }

    /**
     * Module execution function. Receives a pointer to a file the module is to
     * process. The file is represented by a TskFile interface from which both
     * file content and file metadata can be retrieved. Returns TskModule::OK, 
     * TskModule::FAIL, or TskModule::STOP. Returning TskModule::FAIL indicates 
     * the module experienced an error processing the file. Returning TskModule::STOP
     * is a request to terminate processing of the file.
     *
     * @param pFile A pointer to a file to be processed.
     * @returns TskModule::OK on success, TskModule::FAIL on error, or TskModule::STOP.
     */
    TskModule::Status TSK_MODULE_EXPORT run(TskFile *pFile)
    {
        // The TSK Framework convention is to prefix error messages with the
        // name of the module/class and function that emitted the message. 
        std::ostringstream msgPrefix;
        msgPrefix << name() << "::run : ";

        // Well-behaved modules should catch and log all possible exceptions
        // and return an appropriate TskModule::Status to the TSK Framework. 
        TskModule::Status status = TskModule::OK;
        try
        {
            if (pFile == NULL) 
            {
                throw TskException("passed NULL TskFile pointer");
            }

            // Calculate an entropy value for the file.
            double entropy = calculateEntropy(pFile);

            // Post the value to the blackboard.
            pFile->addGenInfoAttribute(TskBlackboardAttribute(TSK_ENTROPY, name(), "", entropy));
        }
        catch (TskException &ex)
        {
            status = TskModule::FAIL;
            std::ostringstream msg;
            msg << msgPrefix.str() << "TskException: " << ex.message();
            LOGERROR(msg.str());
        }
        // Uncomment this catch block and the #include of "Poco/Exception.h" if using
        // Poco.
        //catch (Poco::Exception &ex)
        //{
        //    status = TskModule::FAIL;
        //    std::ostringstream msg;
        //    msg << msgPrefix.str() << "Poco::Exception: " << ex.displayText();
        //    LOGERROR(msg.str());
        //}
        catch (std::exception &ex)
        {
            status = TskModule::FAIL;
            std::ostringstream msg;
            msg << msgPrefix.str() << "std::exception: " << ex.what();
            LOGERROR(msg.str());
        }
        catch (...)
        {
            status = TskModule::FAIL;
            LOGERROR(msgPrefix.str() + "unrecognized exception");
        }

        return status;
    }

    /**
     * Module cleanup function. This is where the module should free any resources 
     * allocated during initialization or execution.
     *
     * @returns TskModule::OK on success and TskModule::FAIL on error.
     */
    TskModule::Status TSK_MODULE_EXPORT finalize()
    {
        // The TSK Framework convention is to prefix error messages with the
        // name of the module/class and function that emitted the message. 
        std::ostringstream msgPrefix;
        msgPrefix << name() << "::finalize : ";

        // Well-behaved modules should catch and log all possible exceptions
        // and return an appropriate TskModule::Status to the TSK Framework. 
        TskModule::Status status = TskModule::OK;
        try
        {
            // If this module required finalization, the finalization code would
            // go here.
        }
        catch (TskException &ex)
        {
            status = TskModule::FAIL;
            std::ostringstream msg;
            msg << msgPrefix.str() << "TskException: " << ex.message();
            LOGERROR(msg.str());
        }
        // Uncomment this catch block and the #include of "Poco/Exception.h" if using
        // Poco.
        //catch (Poco::Exception &ex)
        //{
        //    status = TskModule::FAIL;
        //    std::ostringstream msg;
        //    msg << msgPrefix.str() << "Poco::Exception: " << ex.displayText();
        //    LOGERROR(msg.str());
        //}
        catch (std::exception &ex)
        {
            status = TskModule::FAIL;
            std::ostringstream msg;
            msg << msgPrefix.str() << "std::exception: " << ex.what();
            LOGERROR(msg.str());
        }
        catch (...)
        {
            status = TskModule::FAIL;
            LOGERROR(msgPrefix.str() + "unrecognized exception");
        }

        return status;
    }
}