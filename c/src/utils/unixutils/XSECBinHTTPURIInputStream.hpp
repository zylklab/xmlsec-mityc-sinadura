/*
 * Copyright 2002-2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * XSEC
 *
 * XSECBinHTTPURIInputStream := Re-implementation of Xerces BinHTTPInputStream
 *                              Allows us to modify and create an input
 *                              stream that follows re-directs which is
 *                              necessary to fully support XML-DSIG interop
 *                              tests
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECBinHTTPURIInputStream.hpp 351214 2005-02-03 13:58:14Z milan $
 *
 * $Log$
 * Revision 1.5  2005/02/03 13:55:08  milan
 * Apache licence fix.
 *
 * Revision 1.4  2004/02/08 10:50:22  blautenb
 * Update to Apache 2.0 license
 *
 * Revision 1.3  2003/09/11 11:29:12  blautenb
 * Fix Xerces namespace usage in *NIX build
 *
 * Revision 1.2  2003/07/05 10:30:38  blautenb
 * Copyright update
 *
 * Revision 1.1  2003/02/12 11:21:03  blautenb
 * UNIX generic URI resolver
 *
 *
 */

#ifndef UNIXXSECBINHTTPURIINPUTSTREAM_HEADER
#define UNIXXSECBINHTTPURIINPUTSTREAM_HEADER

#include <xsec/framework/XSECDefs.hpp>

#include <xercesc/util/XMLUri.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/BinInputStream.hpp>

//
// This class implements the BinInputStream interface specified by the XML
// parser.
//

class DSIG_EXPORT XSECBinHTTPURIInputStream : public XERCES_CPP_NAMESPACE_QUALIFIER BinInputStream
{
public :
    XSECBinHTTPURIInputStream(const XERCES_CPP_NAMESPACE_QUALIFIER XMLUri&  urlSource);
    ~XSECBinHTTPURIInputStream();

    unsigned int curPos() const;
    unsigned int readBytes
    (
                XMLByte* const  toFill
        , const unsigned int    maxToRead
    );


private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSocket
    //      The socket representing the connection to the remote file.
    //  fBytesProcessed
    //      Its a rolling count of the number of bytes processed off this
    //      input stream.
    //  fBuffer
    //      Holds the http header, plus the first part of the actual
    //      data.  Filled at the time the stream is opened, data goes
    //      out to user in response to readBytes().
    //  fBufferPos, fBufferEnd
    //      Pointers into fBuffer, showing start and end+1 of content
    //      that readBytes must return.
    // -----------------------------------------------------------------------

	int getSocketHandle(const XERCES_CPP_NAMESPACE_QUALIFIER XMLUri&  urlSource);

    int                 fSocket;
    unsigned int        fBytesProcessed;
    char                fBuffer[4000];
    char *              fBufferEnd;
    char *              fBufferPos;

};


inline unsigned int XSECBinHTTPURIInputStream::curPos() const
{
    return fBytesProcessed;
}


#endif // UNIXXSECBINHTTPURIINPUTSTREAM_HEADER
