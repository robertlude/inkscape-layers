
/////////////////////////////////////
//                          _____  //
//  inkscape-layers        /    /  //
//                        /____//  //
//  by Robert Lude        /____//  //
//    <rob@ertlu.de>      /____/   //
//                                 //
/////////////////////////////////////


#include <libxml++/libxml++.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#define NAME_MODE_DEFAULT 0
#define NAME_MODE_LABEL   1
#define NAME_MODE_ID      2
#define NAME_MODE_INDEX   3

#define VERSION_STRING "0.1.0"

  class
LayerInfo
  { public:
    const std::string label;
    const std::string id;

    LayerInfo
      (  std::string  id
      ,  std::string  label
      )
      :    id ( id )
      , label ( label )
      { }
  };

  class
Mode
  { public:

    ;; /* Variables */ ;;;;;;;;;;;;;;;;;;;
    ;         bool  valid                ;
    ;  std::string  inputFilename        ;
    ;  std::string  layerFilenamePrefix  ;
    ;  std::string  layerFilenameSuffix  ;
    ;          int  nameMode             ;
    ;  std::string  outputDirectory      ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    Mode
      (     int  argumentCount
      ,  char**  arguments
      )
      { ;; /* Local variables */ ;;;;;;;;;;;;;;;;;;;;
        ;                     int  argumentIndex    ;
        ;  std::string::size_type  extensionStart   ;
        ;             std::string  outputBaseName   ;
        ;             std::string  outputDirectory  ;
        ;  std::string::size_type  pathEnd          ;
        ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        // Make sure we have at least one argument

        if ( argumentCount < 2 ) {
          valid = false;
          return;
        }

        // Process arguments

        nameMode = NAME_MODE_DEFAULT;

        for ( argumentIndex = 1 ; argumentIndex < argumentCount ; ++argumentIndex )
            { if ( strcmp(arguments[argumentIndex], "-o") == 0 )
                 { if (  argumentIndex + 1 == argumentCount
                      || outputDirectory.length() != 0
                      )
                      { valid = false;
                        return;
                      }

                   outputDirectory = arguments[argumentIndex + 1];
                   if ( outputDirectory[outputDirectory.length()] != '/' ) outputDirectory += '/';

                   ++argumentIndex;
                   continue;
                 }

              if ( strcmp(arguments[argumentIndex], "-n") == 0 )
                 { if (  argumentIndex + 1 == argumentCount
                      || nameMode != NAME_MODE_DEFAULT
                      )
                      { valid = false;
                        return;
                      }

                   if ( strcmp(arguments[argumentIndex + 1], "label") == 0 )
                      { nameMode = NAME_MODE_LABEL;
                        ++argumentIndex;
                        continue;
                      }

                   if ( strcmp(arguments[argumentIndex + 1], "id") == 0 )
                      { nameMode = NAME_MODE_ID;
                        ++argumentIndex;
                        continue;
                      }

                   if ( strcmp(arguments[argumentIndex + 1], "index") == 0 )
                      { nameMode = NAME_MODE_INDEX;
                        ++argumentIndex;
                        continue;
                      }

                   valid = false;
                   return;
                 }

              if ( strcmp(arguments[argumentIndex], "--name-label") == 0 )
                 { if ( nameMode != NAME_MODE_DEFAULT )
                      { valid = false;
                        return;
                      }
                   nameMode = NAME_MODE_LABEL;
                   continue;
                 }

              if ( strcmp(arguments[argumentIndex], "--name-id") == 0 )
                 { if ( nameMode != NAME_MODE_DEFAULT )
                      { valid = false;
                        return;
                      }
                   nameMode = NAME_MODE_ID;
                   continue;
                 }

              if ( strcmp(arguments[argumentIndex], "--name-index") == 0 )
                 { if ( nameMode != NAME_MODE_DEFAULT )
                      { valid = false;
                        return;
                      }
                   nameMode = NAME_MODE_INDEX;
                   continue;
                 }

              if ( inputFilename.length() != 0 )
                 { valid = false;
                   return;
                 }

              inputFilename = arguments[argumentIndex];
            }

        if ( nameMode == NAME_MODE_DEFAULT ) nameMode = NAME_MODE_LABEL;

        // Extract filename parts

        pathEnd = inputFilename.rfind('/');
        if ( pathEnd == std::string::npos ) pathEnd = 0;
        extensionStart = inputFilename.find('.', pathEnd);

        if ( outputDirectory.length() == 0 ) outputDirectory = inputFilename.substr(0, pathEnd);

        outputBaseName = inputFilename.substr(pathEnd, extensionStart - pathEnd);
        layerFilenamePrefix = outputDirectory + outputBaseName + "-";

        if ( extensionStart == std::string::npos ) {
          layerFilenameSuffix = "";
        } else {
          layerFilenameSuffix = inputFilename.substr(extensionStart);
        }

        // Finish and finalize

        valid = true;
      }
  };

  void
printUsage
  (  )
  { std::cout << "inkscape-layers v" << VERSION_STRING << std::endl
              << std::endl
              << "Usage: inkscape-layers input_file [-o output_directory] [-n name_mode | --name-label | --name-id | --name-index ]" << std::endl
              << std::endl
              << "name_mode may be one of the following:" << std::endl
              << "  label (--name-label) Default. Uses the user-defined layer name from Inkscape" << std::endl
              << "  id    (--name-id)    Uses the layer's id attribute" << std::endl
              << "  index (--name-index) Uses the layer's position in the svg file" << std::endl
              ;
  }

  std::vector<LayerInfo>*
acquireLayerInfo
  (           xmlpp::Document*  document
  ,  xmlpp::Node::PrefixNsMap&  prefixNsMap
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                 xmlpp::Element*  rootNode       ;
    ;                  xmlpp::NodeSet  layers         ;
    ;  xmlpp::NodeSet::const_iterator  layerIterator  ;
    ;         std::vector<LayerInfo>*  result         ;
    ;                 xmlpp::Element*  layer          ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    rootNode = document->get_root_node();

    layers = rootNode->find("//svg:g[@inkscape:groupmode='layer']", prefixNsMap);

    result = new std::vector<LayerInfo>();

    layerIterator = layers.begin();
    while ( layerIterator != layers.end() )
          { layer = (xmlpp::Element*)(*layerIterator);
            result->push_back ( LayerInfo ( std::string(layer->get_attribute_value("id").raw())
                                          , std::string(layer->get_attribute_value("label","inkscape").raw())
                                          )
                              );
            ++layerIterator;
          }

    return result;
  }

  void
isolate
  (                std::string  layerId
  ,            xmlpp::Element*  rootNode
  ,  xmlpp::Node::PrefixNsMap&  prefixNsMap
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                  xmlpp::NodeSet  layers         ;
    ;  xmlpp::NodeSet::const_iterator  layerIterator  ;
    ;                 xmlpp::Element*  element        ;
    ;                     std::string  xPath          ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    layers = rootNode->find(std::string("//svg:g[@inkscape:groupmode='layer' and @id!='") + layerId + "']", prefixNsMap);

    std::cout << "Extracting layer " << layerId << std::endl;

    layerIterator = layers.begin();
    while ( layerIterator != layers.end() )
          { rootNode->get_parent()->remove_child(*layerIterator);
            ++layerIterator;
          }
  }

  int
main
  (     int  argumentCount
  ,  char**  arguments
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                  xmlpp::DomParser  domParser            ;
    ;           std::vector<LayerInfo>*  layerInfo            ;
    ;  std::vector<LayerInfo>::iterator  layerIterator        ;
    ;          xmlpp::Node::PrefixNsMap  prefixNsMap          ;
    ;                       std::string  layerOutputFilename  ;
    ;                  xmlpp::Document*  document             ;
    ;                               int  layerIndex           ;
    ;                             Mode*  mode                 ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    // Process arguments

    mode = new Mode(argumentCount, arguments);

    if ( ! mode->valid )
       { printUsage();
         return -1;
       }

    // Setup XML namespaces

    prefixNsMap["svg"] = "http://www.w3.org/2000/svg";
    prefixNsMap["inkscape"] = "http://www.inkscape.org/namespaces/inkscape";

    // Read layer names

    domParser.parse_file(mode->inputFilename);
    document = domParser.get_document();

    layerInfo = acquireLayerInfo(document, prefixNsMap);

    // Extract each layer

    layerIterator = layerInfo->begin();
    layerIndex = 0;
    while ( layerIterator != layerInfo->end() )
          { domParser.parse_file(mode->inputFilename);

            switch ( mode->nameMode )
                   { case NAME_MODE_LABEL : layerOutputFilename = mode->layerFilenamePrefix + layerIterator->label + mode->layerFilenameSuffix;
                                            break;
                     case    NAME_MODE_ID : layerOutputFilename = mode->layerFilenamePrefix + layerIterator->id + mode->layerFilenameSuffix;
                                            break;
                     case NAME_MODE_INDEX : layerOutputFilename = mode->layerFilenamePrefix + std::to_string(layerIndex) + mode->layerFilenameSuffix;
                                            break;
                   }

            document = domParser.get_document();
            isolate(layerIterator->id, document->get_root_node(), prefixNsMap);
            document->write_to_file(layerOutputFilename);
            ++layerIterator;
            ++layerIndex;
          }

    // Clean up

    delete layerInfo;
    delete mode;

    // Finish

    return 0;
  }
