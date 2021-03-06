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
 * DSIGTransformXSL := Class that Handles DSIG XSLT Transforms
 *
 * $Id: DSIGTransformXSL.cpp 409902 2006-05-28 01:14:18Z blautenb $
 *
 */

// XSEC

#include <xsec/dsig/DSIGTransformXSL.hpp>
#include <xsec/dsig/DSIGSignature.hpp>
#include <xsec/transformers/TXFMXSL.hpp>
#include <xsec/transformers/TXFMC14n.hpp>
#include <xsec/transformers/TXFMChain.hpp>
#include <xsec/framework/XSECException.hpp>
#include <xsec/framework/XSECEnv.hpp>
#include <xsec/utils/XSECDOMUtils.hpp>
#include <xsec/framework/XSECError.hpp>

XERCES_CPP_NAMESPACE_USE

// --------------------------------------------------------------------------------
//           Constructors and Destructors
// --------------------------------------------------------------------------------

DSIGTransformXSL::DSIGTransformXSL(const XSECEnv * env, DOMNode * node) :
DSIGTransform(env, node),
mp_stylesheetNode(NULL) {};


DSIGTransformXSL::DSIGTransformXSL(const XSECEnv * env) :
DSIGTransform(env),
mp_stylesheetNode(NULL) {};
		  

DSIGTransformXSL::~DSIGTransformXSL() {};

// --------------------------------------------------------------------------------
//           Interface Methods
// --------------------------------------------------------------------------------


transformType DSIGTransformXSL::getTransformType() {

	return TRANSFORM_XSLT;

}


void DSIGTransformXSL::appendTransformer(TXFMChain * input) {


#ifdef XSEC_NO_XSLT

	throw XSECException(XSECException::UnsupportedFunction,
		"XSLT Transforms not supported in this compilation of the library");
#else

	if (mp_stylesheetNode == 0)
		throw XSECException(XSECException::XSLError, "Style Sheet not found for XSL Transform");


	TXFMBase * nextInput;

	// XSLT Transform - requires a byte stream input
	
	if (input->getLastTxfm()->getOutputType() == TXFMBase::DOM_NODES) {
		
		// Use c14n to translate to BYTES
		
		XSECnew(nextInput, TXFMC14n(mp_txfmNode->getOwnerDocument()));
		input->appendTxfm(nextInput);
	}

	TXFMXSL * x;
	
	// Create the XSLT transform
	XSECnew(x, TXFMXSL(mp_txfmNode->getOwnerDocument()));
	input->appendTxfm(x);
	
	// Again use C14n (convenient) to translate to a SafeBuffer
	
	XSECC14n20010315 c14n(mp_txfmNode->getOwnerDocument(), mp_stylesheetNode);
	safeBuffer sbStyleSheet;
	unsigned int size, count;
	unsigned char buf[512];
	size = 0;
	
	while ((count = c14n.outputBuffer(buf, 512)) != 0) {
		
		sbStyleSheet.sbMemcpyIn(size, buf, count);
		size += count;
		
	}
	
	sbStyleSheet[size] = '\0';		// Terminate as though a string
	
	x->evaluateStyleSheet(sbStyleSheet);

#endif /* NO_XSLT */

}


DOMElement * DSIGTransformXSL::createBlankTransform(DOMDocument * parentDoc) {

	safeBuffer str;
	const XMLCh * prefix;
	DOMElement *ret;
	DOMDocument *doc = mp_env->getParentDocument();

	prefix = mp_env->getDSIGNSPrefix();
	
	// Create the transform node
	makeQName(str, prefix, "Transform");
	ret = doc->createElementNS(DSIGConstants::s_unicodeStrURIDSIG, str.rawXMLChBuffer());
	ret->setAttributeNS(NULL,DSIGConstants::s_unicodeStrAlgorithm, DSIGConstants::s_unicodeStrURIXSLT);

	mp_txfmNode = ret;
	mp_stylesheetNode = NULL;

	return ret;

}

void DSIGTransformXSL::load(void) {

	// find the style sheet
	mp_stylesheetNode = mp_txfmNode->getFirstChild();
	while (mp_stylesheetNode != 0 && 
		mp_stylesheetNode->getNodeType() != DOMNode::ELEMENT_NODE && !strEquals(mp_stylesheetNode->getNodeName(), "xsl:stylesheet"))
		mp_stylesheetNode = mp_stylesheetNode->getNextSibling();

	if (mp_stylesheetNode == 0)
		throw XSECException(XSECException::XSLError, "Style Sheet not found for XSL Transform");


}
// --------------------------------------------------------------------------------
//           XSLT Specific Methods
// --------------------------------------------------------------------------------

DOMNode * DSIGTransformXSL::setStylesheet(DOMNode * stylesheet) {

	DOMNode * ret = mp_stylesheetNode;

	if (mp_stylesheetNode != 0) {
		mp_txfmNode->insertBefore(stylesheet, mp_stylesheetNode);
		mp_txfmNode->removeChild(mp_stylesheetNode);
	}

	mp_stylesheetNode = stylesheet;

	return ret;

}

DOMNode * DSIGTransformXSL::getStylesheet(void) {

	return mp_stylesheetNode;

}
