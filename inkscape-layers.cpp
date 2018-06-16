
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

#define VISIBILITY_MODE_DEFAULT 0
#define VISIBILITY_MODE_ALL     1
#define VISIBILITY_MODE_VISIBLE 2

#define VERSION_STRING "0.2.1"

  class
LayerInfo
  { public:

    ;; /* Variables */ ;;;;;;;;;;;;;;
    ;  const  std::string  label    ;
    ;  const  std::string  id       ;
    ;  const         bool  visible  ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    LayerInfo
      (  std::string  id
      ,  std::string  label
      ,         bool  visible
      )
      :      id ( id )
      ,   label ( label )
      , visible ( visible )
      { }
  };

  typedef
  std::vector<LayerInfo*>
LayerList
  ;

  class
Mode
  { public:

    ;; /* Variables */ ;;;;;;;;;;;;;;;;;;;
    ;         bool  valid                ;
    ;  std::string  inputFilename        ;
    ;  std::string  layerFilenamePrefix  ;
    ;  std::string  layerFilenameSuffix  ;
    ;          int  nameMode             ;
    ;          int  visibilityMode       ;
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

        valid = false;

        // Make sure we have at least one argument

        if ( argumentCount < 2 ) return;

        // Process arguments

        nameMode = NAME_MODE_DEFAULT;
        visibilityMode = NAME_MODE_DEFAULT;

        for ( argumentIndex = 1 ; argumentIndex < argumentCount ; ++argumentIndex )
            { // output directory

              if (  strcmp(arguments[argumentIndex], "-o") == 0
                 || strcmp(arguments[argumentIndex], "--output-directory") == 0
                 )
                 { if (  argumentIndex + 1 == argumentCount
                      || outputDirectory.length() != 0
                      )
                      { return; }

                   outputDirectory = arguments[argumentIndex + 1];
                   if ( outputDirectory[outputDirectory.length()] != '/' ) outputDirectory += '/';

                   ++argumentIndex;
                   continue;
                 }

              // name mode

              if ( strcmp(arguments[argumentIndex], "-n") == 0 )
                 { if (  argumentIndex + 1 == argumentCount
                      || nameMode != NAME_MODE_DEFAULT
                      )
                      { return; }

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

                   return;
                 }

              if ( strcmp(arguments[argumentIndex], "--name-label") == 0 )
                 { if ( nameMode != NAME_MODE_DEFAULT ) return;
                   nameMode = NAME_MODE_LABEL;
                   continue;
                 }

              if ( strcmp(arguments[argumentIndex], "--name-id") == 0 )
                 { if ( nameMode != NAME_MODE_DEFAULT ) return;
                   nameMode = NAME_MODE_ID;
                   continue;
                 }

              if ( strcmp(arguments[argumentIndex], "--name-index") == 0 )
                 { if ( nameMode != NAME_MODE_DEFAULT ) return;
                   nameMode = NAME_MODE_INDEX;
                   continue;
                 }

              // visibility mode

              if ( strcmp(arguments[argumentIndex], "-v") == 0 )
                 { if (  argumentIndex + 1 == argumentCount
                      || visibilityMode != VISIBILITY_MODE_DEFAULT
                      )
                      { return; }

                   if ( strcmp(arguments[argumentIndex + 1], "all") == 0 )
                      { visibilityMode = VISIBILITY_MODE_ALL;
                        ++argumentIndex;
                        continue;
                      }

                   if ( strcmp(arguments[argumentIndex + 1], "visible") == 0 )
                      { visibilityMode = VISIBILITY_MODE_VISIBLE;
                        ++argumentIndex;
                        continue;
                      }

                   return;
                 }

              if ( strcmp(arguments[argumentIndex], "--visibility-all") == 0 )
                 { if ( visibilityMode != VISIBILITY_MODE_DEFAULT ) return;
                   visibilityMode = VISIBILITY_MODE_ALL;
                   continue;
                 }

              if ( strcmp(arguments[argumentIndex], "--visibility-visible") == 0 )
                 { if ( visibilityMode != VISIBILITY_MODE_DEFAULT ) return;
                   visibilityMode = VISIBILITY_MODE_VISIBLE;
                   continue;
                 }

              // input file

              if ( inputFilename.length() != 0 )
                 { valid = false;
                   return;
                 }

              inputFilename = arguments[argumentIndex];
            }

        if ( nameMode == NAME_MODE_DEFAULT ) nameMode = NAME_MODE_LABEL;
        if ( visibilityMode == VISIBILITY_MODE_DEFAULT ) visibilityMode = VISIBILITY_MODE_ALL;

        // Extract filename parts

        pathEnd = inputFilename.rfind('/');
        if ( pathEnd == std::string::npos ) pathEnd = 0;
        extensionStart = inputFilename.find('.', pathEnd);

        if ( outputDirectory.length() == 0 ) outputDirectory = inputFilename.substr(0, pathEnd);

        outputBaseName = inputFilename.substr(pathEnd, extensionStart - pathEnd);

        /* HACK :( */
        /* commented for filename without prefix
        layerFilenamePrefix = outputDirectory + outputBaseName + "-";
        */

        layerFilenamePrefix = outputDirectory;

        /*KCAH :( */

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
              << "  by Robert Lude <rob@ertlu.de>" << std::endl
              << std::endl
              << "Usage:" << std::endl
              << "  inkscape-layers input_file options" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "  -o output_directory, --output-directory output_directory" << std::endl
              << "    Specifies where to put the resulting SVG files" << std::endl
              << "  -n name_mode, --name-label, --name-id, --name-index" << std::endl
              << "    Specifies how to name the resulting SVG files. Acceptable values for name_mode are:" << std::endl
              << "      label   Default. Uses the user-defined layer name from Inkscape" << std::endl
              << "      id      Uses the layer's id attribute" << std::endl
              << "      index   Uses the layer's position in the original SVG file" << std::endl
              << "  -v visibility_mode, --visibility-all, --visibility-visible" << std::endl
              << "    Specifies whether or not to skip invisible layers, or rather which layers to isolate." << std::endl
              << "    Acceptable values for visiblity_mode are:" << std::endl
              << "      visible   Default. Only exports layers which are visible" << std::endl
              << "      all       Exports each layer" << std::endl
              ;
  }

  bool
isVisible
  (  xmlpp::Element*  layer  )
  { ;;/* Local Variables */;;;;;;;;;;;;;;;;;;;
    ;             std::string  style         ;
    ;  std::string::size_type  displayBegin  ;
    ;  std::string::size_type  valueBegin    ;
    ;  std::string::size_type  valueEnd      ;
    ;  std::string::size_type  valueLength   ;
    ;             std::string  value         ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    style = layer->get_attribute_value("style").raw();
    // Trim::inPlace(style);

    if ( (displayBegin = style.find("display")) == std::string::npos ) return true;
    if ( (valueBegin = style.find(':', displayBegin)) == std::string::npos ) return true;
    ++valueBegin;
    valueEnd = style.find(';', valueBegin);
    valueLength = valueEnd - valueBegin;

    value = style.substr(valueBegin, valueLength);
    // Trim::inPlace(value);
    return value.compare("none") != 0;
  }

  void
makeVisible
  (  xmlpp::Element*  layer  )
  { ;;/* Local Variables */;;;;;;;;;;;;;;;;;;;
    ;             std::string  style         ;
    ;  std::string::size_type  displayBegin  ;
    ;  std::string::size_type  valueBegin    ;
    ;  std::string::size_type  valueEnd      ;
    ;  std::string::size_type  valueLength   ;
    ;             std::string  value         ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    style = layer->get_attribute_value("style").raw();
    // Trim::inPlace(style);

    if ( (displayBegin = style.find("display")) == std::string::npos ) return;
    if ( (valueBegin = style.find(':', displayBegin)) == std::string::npos ) return;
    ++valueBegin;
    valueEnd = style.find(';', valueBegin);
    valueLength = valueEnd - valueBegin;

    style.replace(valueBegin, valueLength, "inline");

    layer->set_attribute("style", style);
  }

  LayerList*
acquireLayerInfo
  (           xmlpp::Document*  document
  ,  xmlpp::Node::PrefixNsMap&  prefixNsMap
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                 xmlpp::Element*  rootNode       ;
    ;                  xmlpp::NodeSet  layers         ;
    ;  xmlpp::NodeSet::const_iterator  layerIterator  ;
    ;                      LayerList*  result         ;
    ;                 xmlpp::Element*  layer          ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    rootNode = document->get_root_node();

    layers = rootNode->find("//svg:g[@inkscape:groupmode='layer']", prefixNsMap);

    result = new LayerList();

    layerIterator = layers.begin();
    while ( layerIterator != layers.end() )
          { layer = (xmlpp::Element*)(*layerIterator);
            result->push_back ( new LayerInfo ( std::string(layer->get_attribute_value("id").raw())
                                              , std::string(layer->get_attribute_value("label","inkscape").raw())
                                              , isVisible(layer)
                                              )
                              );
            ++layerIterator;
          }

    return result;
  }

  void
isolate
  (                 LayerInfo*  layer
  ,            xmlpp::Element*  rootNode
  ,  xmlpp::Node::PrefixNsMap&  prefixNsMap
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;                  xmlpp::NodeSet  layers         ;
    ;  xmlpp::NodeSet::const_iterator  layerIterator  ;
    ;                     std::string  xPath          ;
    ;                 xmlpp::Element*  layerElement   ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    // Delete all layers that have a different id

    layers = rootNode->find(std::string("//svg:g[@inkscape:groupmode = 'layer' and @id != '") + layer->id + "']", prefixNsMap);

    for ( layerIterator = layers.begin()
        ; layerIterator != layers.end()
        ; ++layerIterator
        )
        { rootNode->get_parent()->remove_child(*layerIterator); }

    // Make remaining layers visible if necessary

    if ( ! layer->visible )
       { layers = rootNode->find(std::string("//svg:g[@inkscape:groupmode = 'layer' and @id = '") + layer->id + "']", prefixNsMap);
         for ( layerIterator = layers.begin()
             ; layerIterator != layers.end()
             ; ++layerIterator
             )
             { if ( (layerElement = dynamic_cast<xmlpp::Element*>(*layerIterator)) ) makeVisible(layerElement); }
       }
  }

  int
main
  (     int  argumentCount
  ,  char**  arguments
  )
  { ;; /* Local Variables */ ;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;          xmlpp::DomParser  domParser            ;
    ;                LayerList*  layerList            ;
    ;       LayerList::iterator  layerIterator        ;
    ;  xmlpp::Node::PrefixNsMap  prefixNsMap          ;
    ;               std::string  layerOutputFilename  ;
    ;               std::string  layerOutputName      ;
    ;          xmlpp::Document*  document             ;
    ;                       int  layerIndex           ;
    ;                     Mode*  mode                 ;
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    // Process arguments

    mode = new Mode(argumentCount, arguments);

    if ( ! mode->valid )
       { printUsage();
         return -1;
       }

    // Setup XML namespaces

    prefixNsMap["svg"] = "http://www.w3.org/2000/svg";
    prefixNsMap["inkscape"] = "http://www.inkscape.org/namespaces/inkscape";

    // Read layer info

    domParser.parse_file(mode->inputFilename);
    document = domParser.get_document();

    layerList = acquireLayerInfo(document, prefixNsMap);

    // Extract each layer

    layerIndex = 0;
    for ( layerIndex = 0
        , layerIterator = layerList->begin()
        ; layerIndex < layerList->size()
        ; ++layerIndex
        , ++layerIterator
        )
        { if (  mode->visibilityMode == VISIBILITY_MODE_VISIBLE
             && ! (*layerIterator)->visible
             )
             { continue; }

          domParser.parse_file(mode->inputFilename);

          switch ( mode->nameMode )
                 { case NAME_MODE_LABEL : layerOutputName = (*layerIterator)->label;
                                          break;
                   case    NAME_MODE_ID : layerOutputName = (*layerIterator)->id;
                                          break;
                   case NAME_MODE_INDEX : layerOutputName = std::to_string(layerIndex);
                                          break;
                 }

          layerOutputFilename = ( mode->layerFilenamePrefix
                                + layerOutputName
                                + mode->layerFilenameSuffix
                                );

          document = domParser.get_document();
          isolate(*layerIterator, document->get_root_node(), prefixNsMap);
          document->write_to_file(layerOutputFilename);
        }

    // Clean up

    for ( layerIterator = layerList->begin()
        ; layerIterator != layerList->end()
        ; ++layerIterator
        )
        { delete *layerIterator; }

    delete layerList;
    delete mode;

    // Finish

    return 0;
  }
