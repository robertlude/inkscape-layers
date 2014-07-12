
#include <libxml++/libxml++.h>
#include <iostream>
#include <stdlib.h>

  void
printUsage
  (  )
  { std::cout << "Usage: inkscape-layers inputfile" << std::endl; }

  std::string
getFilepathBase
  (  std::string  filename  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;
    ;  std::string::size_type  pathEnd         ;
    ;  std::string::size_type  extensionStart  ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    if (  (pathEnd = filename.rfind('/'))
       == std::string::npos
       )
       { pathEnd = 0; }
    if (  (extensionStart = filename.find('.', pathEnd))
       == std::string::npos
       )
       { extensionStart = filename.length(); }

    return filename.substr(0, extensionStart);
  }

  std::vector<std::string>*
acquireLayerNames
  (           xmlpp::Document*  document
  ,  xmlpp::Node::PrefixNsMap*  prefixNsMap
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                 xmlpp::Element*  rootNode       ;
    ;                  xmlpp::NodeSet  layers         ;
    ;  xmlpp::NodeSet::const_iterator  layerIterator  ;
    ;       std::vector<std::string>*  result         ;
    ;                 xmlpp::Element*  layer          ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    rootNode = document->get_root_node();

    layers = rootNode->find("//svg:g[@inkscape:groupmode='layer']", *prefixNsMap);

    result = new std::vector<std::string>();

    layerIterator = layers.begin();
    while ( layerIterator != layers.end() )
          { layer = (xmlpp::Element*)(*layerIterator);
            result->push_back( std::string(layer->get_attribute_value("label","inkscape").raw()) );
            ++layerIterator;
          }

    return result;
  }

  void
isolate
  (                std::string  layerName
  ,            xmlpp::Element*  rootNode
  ,  xmlpp::Node::PrefixNsMap*  prefixNsMap
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                  xmlpp::NodeSet  layers         ;
    ;  xmlpp::NodeSet::const_iterator  layerIterator  ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    layers = rootNode->find(std::string("//svg:g[@inkscape:label != '") + layerName + "']", *prefixNsMap);

    layerIterator = layers.begin();
    while ( layerIterator != layers.end() )
          { rootNode->get_parent()->remove_child(*layerIterator);
            ++layerIterator;
          }
  }

  int
main
  (     int  argumentCount
  ,  char**  argument
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                   xmlpp::DomParser*  domParser            ;
    ;           std::vector<std::string>*  layerNames           ;
    ;  std::vector<std::string>::iterator  layerIterator        ;
    ;                         std::string  inputFilename        ;
    ;                         std::string  layerFilenamePrefix  ;
    ;                         std::string  layerFilenameSuffix  ;
    ;              std::string::size_type  pathEnd              ;
    ;              std::string::size_type  extensionStart       ;
    ;           xmlpp::Node::PrefixNsMap*  prefixNsMap          ;
    ;                         std::string  layerOutputFilename  ;
    ;                    xmlpp::Document*  document             ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    if ( argumentCount != 2 )
       { printUsage();
         return -1;
       }

    inputFilename = argument[1];

    domParser = new xmlpp::DomParser(inputFilename);
    document = domParser->get_document();

    prefixNsMap = new xmlpp::Node::PrefixNsMap();
    (*prefixNsMap)["svg"] = "http://www.w3.org/2000/svg";
    (*prefixNsMap)["inkscape"] = "http://www.inkscape.org/namespaces/inkscape";

    layerNames = acquireLayerNames(document, prefixNsMap);

    pathEnd = inputFilename.rfind('/');
    if ( pathEnd == std::string::npos ) pathEnd = 0;
    extensionStart = inputFilename.find('.', pathEnd);

    if ( extensionStart == std::string::npos ) {
      layerFilenamePrefix = inputFilename;
      layerFilenameSuffix = "";
    } else {
      layerFilenamePrefix = inputFilename.substr(0, extensionStart);
      layerFilenameSuffix = inputFilename.substr(extensionStart);
    }

    layerIterator = layerNames->begin();
    while ( layerIterator != layerNames->end() )
          { domParser->parse_file(inputFilename);
            layerOutputFilename = layerFilenamePrefix + "-" + (*layerIterator) + layerFilenameSuffix;
            document = domParser->get_document();
            isolate(*layerIterator, document->get_root_node(), prefixNsMap);
            document->write_to_file(layerOutputFilename);
            ++layerIterator;
          }

    delete prefixNsMap;
    delete layerNames;
    delete domParser;

    return 0;
  }
