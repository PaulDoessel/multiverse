//-*****************************************************************************
//
// Copyright (c) 2013,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif
#include <Alembic/AbcCoreOgawa/All.h>
#ifdef ALEMBIC_WITH_MULTIVERSE
#include <Alembic/AbcCoreGit/All.h>
#endif

#include <boost/algorithm/string/predicate.hpp>

void copyProps(Alembic::Abc::ICompoundProperty & iRead,
    Alembic::Abc::OCompoundProperty & iWrite)
{
    std::size_t numChildren = iRead.getNumProperties();
    for (std::size_t i = 0; i < numChildren; ++i)
    {
        Alembic::AbcCoreAbstract::PropertyHeader header =
            iRead.getPropertyHeader(i);
        if (header.isArray())
        {
            Alembic::Abc::IArrayProperty inProp(iRead, header.getName());
            Alembic::Abc::OArrayProperty outProp(iWrite, header.getName(),
                header.getDataType(), header.getMetaData(),
                header.getTimeSampling());

            std::size_t numSamples = inProp.getNumSamples();

            for (std::size_t j = 0; j < numSamples; ++j)
            {
                Alembic::AbcCoreAbstract::ArraySamplePtr samp;
                Alembic::Abc::ISampleSelector sel(
                    (Alembic::Abc::index_t) j);
                inProp.get(samp, sel);
                outProp.set(*samp);
            }
        }
        else if (header.isScalar())
        {
            Alembic::Abc::IScalarProperty inProp(iRead, header.getName());
            Alembic::Abc::OScalarProperty outProp(iWrite, header.getName(),
                header.getDataType(), header.getMetaData(),
                header.getTimeSampling());

            std::size_t numSamples = inProp.getNumSamples();
            std::vector<std::string> sampStrVec;
            std::vector<std::wstring> sampWStrVec;
            if (header.getDataType().getPod() ==
                Alembic::AbcCoreAbstract::kStringPOD)
            {
                sampStrVec.resize(header.getDataType().getExtent());
            }
            else if (header.getDataType().getPod() ==
                     Alembic::AbcCoreAbstract::kWstringPOD)
            {
                sampWStrVec.resize(header.getDataType().getExtent());
            }

            char samp[4096];

            for (std::size_t j = 0; j < numSamples; ++j)
            {
                Alembic::Abc::ISampleSelector sel(
                    (Alembic::Abc::index_t) j);

                if (header.getDataType().getPod() ==
                    Alembic::AbcCoreAbstract::kStringPOD)
                {
                    inProp.get(&sampStrVec.front(), sel);
                    outProp.set(&sampStrVec.front());
                }
                else if (header.getDataType().getPod() ==
                    Alembic::AbcCoreAbstract::kWstringPOD)
                {
                    inProp.get(&sampWStrVec.front(), sel);
                    outProp.set(&sampWStrVec.front());
                }
                else
                {
                    inProp.get(samp, sel);
                    outProp.set(samp);
                }
            }
        }
        else if (header.isCompound())
        {
            Alembic::Abc::OCompoundProperty outProp(iWrite,
                header.getName(), header.getMetaData());
            Alembic::Abc::ICompoundProperty inProp(iRead, header.getName());
            copyProps(inProp, outProp);
        }
    }
}

void copyObject(Alembic::Abc::IObject & iIn,
    Alembic::Abc::OObject & iOut)
{
    std::size_t numChildren = iIn.getNumChildren();

    Alembic::Abc::ICompoundProperty inProps = iIn.getProperties();
    Alembic::Abc::OCompoundProperty outProps = iOut.getProperties();
    copyProps(inProps, outProps);

    for (std::size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::IObject childIn(iIn.getChild(i));
        Alembic::Abc::OObject childOut(iOut, childIn.getName(),
                                       childIn.getMetaData());
        copyObject(childIn, childOut);
    }
}

static void usage()
{
    printf ("Usage: abcconvert [-force] [-r | --revision REVISION] [--milliways ON|OFF] [-m | --message COMMIT-MESSAGE] OPTION inFile outFile\n");
    printf ("Used to convert an Alembic file from one type to another.\n\n");
    printf ("If -force is not provided and inFile happens to be the same\n");
    printf ("type as OPTION no conversion will be done and a message will\n");
    printf ("be printed out.\n");
    printf ("OPTION has to be one of these:\n\n");
#ifdef ALEMBIC_WITH_HDF5
    printf ("  -toHDF   Convert to HDF.\n");
#endif
    printf ("  -toOgawa Convert to Ogawa.\n");
#ifdef ALEMBIC_WITH_MULTIVERSE
    printf ("  -toGit   Convert to Git.\n");
#endif
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    std::string toType;
    std::string inFile;
    std::string outFile;
    std::string revision;
    bool milliways = false;
    bool forceOpt = false;

    std::string commitMessage;

    int arg_i = 1;
    while (arg_i < argc)
    {
        std::string arg = argv[arg_i];
        if (! boost::starts_with(arg, "-"))
            break;

        if ((arg == "-force") || (arg == "--force"))
        {
            forceOpt = true;
        } else if ((arg == "-toHDF") || (arg == "-toOgawa") || (arg == "-toGit"))
        {
            toType = arg;
        } else if ((arg == "-m") || (arg == "--message") || boost::starts_with(arg, "--message="))
        {
            if (boost::starts_with(arg, "--message="))
            {
                commitMessage = arg.substr(10, std::string::npos);
            } else
            {
                commitMessage = argv[++arg_i];
            }
        } else if ((arg == "-r") || (arg == "--revision") || boost::starts_with(arg, "--revision="))
        {
            if (boost::starts_with(arg, "--revision="))
            {
                revision = arg.substr(11, std::string::npos);
            } else
            {
                revision = argv[++arg_i];
            }
        } else if ((arg == "--mw") || (arg == "--milliways") || boost::starts_with(arg, "--milliways="))
        {
            std::string opt_value;
            if (boost::starts_with(arg, "--milliways="))
            {
                opt_value = arg.substr(12, std::string::npos);
            } else
            {
                opt_value = argv[++arg_i];
            }
            if ((opt_value == "ON") || (opt_value == "on") || (opt_value == "true"))
                milliways = true;
            else
                milliways = false;
        }

        ++arg_i;
    }

    if ((arg_i + 1) >= argc)
        usage();

    inFile = argv[arg_i++];
    outFile = argv[arg_i++];

    // std::cout << "to:" << toType << " force:" << forceOpt << " revision:" << revision << " message:'" << commitMessage << "' inFile:'" << inFile << "' outFile:'" << outFile << "'" << std::endl;

    if ((inFile == outFile) && (toType != "-toGit"))
    {
        printf("Error: inFile and outFile must not be the same!\n");
        return 1;
    }

    if (toType != "-toHDF" && toType != "-toOgawa" && toType != "-toGit")
    {
        printf("Error: Unknown conversion type specified %s\n",
               toType.c_str());
        printf("Currently only -toHDF, -toOgawa and -toGit are supported.\n");
        return 1;
    }

    Alembic::AbcCoreFactory::IOptions rOptions;

    if (! revision.empty())
    {
        rOptions["revision"] = revision;
        // rOptions["ignoreNonexistentRevision"] = true;
    }

    // if (toType == "-toGit")
    //     rOptions["milliways"] = milliways ? true : false;

    Alembic::AbcCoreFactory::IFactory factory;
    Alembic::AbcCoreFactory::IFactory::CoreType coreType;
    Alembic::Abc::IArchive archive = factory.getArchive(inFile, coreType, rOptions);
    if (!archive.valid())
    {
        printf("Error: Invalid Alembic file specified: %s\n",
               inFile.c_str());
        return 1;
    }
    else if ( (!forceOpt) && (
#ifdef ALEMBIC_WITH_HDF5
        (coreType == Alembic::AbcCoreFactory::IFactory::kHDF5 &&
         toType == "-toHDF") ||
#endif
        (coreType == Alembic::AbcCoreFactory::IFactory::kOgawa &&
         toType == "-toOgawa")
#ifdef ALEMBIC_WITH_MULTIVERSE
        || (coreType == Alembic::AbcCoreFactory::IFactory::kGit &&
         toType == "-toGit")
#endif
        ) )
    {
        printf("Warning: Alembic file specified: %s\n",inFile.c_str());
        printf("is already of the type you want to convert to.\n");
        printf("Please specify -force if you want to do this anyway.\n");
        return 1;
    }

    Alembic::Abc::IObject inTop = archive.getTop();
    Alembic::Abc::OArchive outArchive;
#ifdef ALEMBIC_WITH_HDF5
    if (toType == "-toHDF")
    {
        outArchive = Alembic::Abc::OArchive(
            Alembic::AbcCoreHDF5::WriteArchive(),
            outFile, inTop.getMetaData(),
            Alembic::Abc::ErrorHandler::kThrowPolicy);
    } else 
#endif
    if (toType == "-toOgawa")
    {
        outArchive = Alembic::Abc::OArchive(
            Alembic::AbcCoreOgawa::WriteArchive(),
            outFile, inTop.getMetaData(),
            Alembic::Abc::ErrorHandler::kThrowPolicy);
    }
#ifdef ALEMBIC_WITH_MULTIVERSE
    else if (toType == "-toGit")
    {
        Alembic::AbcCoreGit::WriteOptions wOptions;

        if (! commitMessage.empty())
            wOptions.setCommitMessage(commitMessage);

        std::cout << "milliways is " << (milliways ? "enabled" : "disabled") << std::endl;
        wOptions["milliways"] = (milliways ? true : false);

        outArchive = Alembic::Abc::OArchive(
            Alembic::AbcCoreGit::WriteArchive(wOptions),
            outFile, inTop.getMetaData(),
            Alembic::Abc::ErrorHandler::kThrowPolicy);
    }
#endif

    // start at 1, we don't need to worry about intrinsic default case
    for (Alembic::Util::uint32_t i = 1; i < archive.getNumTimeSamplings();
         ++i)
    {
        outArchive.addTimeSampling(*archive.getTimeSampling(i));
    }

    Alembic::Abc::OObject outTop = outArchive.getTop();
    copyObject(inTop, outTop);
    return 0;
}
