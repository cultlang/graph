#pragma once

#include <map>

#include "../util.hpp"

namespace graph
{
    template<typename TType, typename TData>
    struct basic_typed_config
        : basic_core_config<std::tuple<TData, TType>>
    {
        
    };

    /*
     * This graph mixin adds typing to the graph. Each Core object has a type parameter.
     * We pull from the config a set of functionality:
     * - TypeId is the storage type for the type identifier
     * - typed_typeToValue<T>()
     */
    template<typename TGraphBase>
    class GraphTyped
        : public TGraphBase
    {
    // Graph types:
    public:
        using Config = typename TGraphBase::Config;
        using TypeId = typename Config::TypeId;

        struct Core
            : public TGraphBase::Core
        {
            TypeId type;
        };

        struct Label
            : public TGraphBase::Label
        {
            std::map<TypeId, size_t> nodes_type_accel;
        };

        struct Node
            : public TGraphBase::Node
        {
            std::map<TypeId, size_t> edges_type_accel;
            std::map<TypeId, size_t> props_type_accel;
        };

        struct Edge
            : public TGraphBase::Edge
        {
            std::map<TypeId, size_t> props_type_accel;
        };
        
    // Actual type:
    public:
        template<typename TFinal>
        class Actual
            : private TGraphBase::template Actual<TFinal>
        {
            using Base = typename TGraphBase::template Actual<TFinal>;

        public:
            using MetaFlags = typename TFinal::MetaFlags;
            using CoreData = typename TFinal::CoreData;

            using Label = typename TFinal::Label;
            using Node = typename TFinal::Node;
            using Edge = typename TFinal::Edge;
            using Prop = typename TFinal::Prop;

        // Add functions
        public:
            inline Label* addLabel(CoreData const& data, TypeId type)
            {
                auto ret = Base::addLabel(data);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Label* addLabel(T const& data)
            {
                return addLabel(Config::template typed_store<T>(data), Config::template typed_typeToValue<T>());
            }

            inline Node* addNode(CoreData const& data, TypeId type)
            {
                auto ret = Base::addNode(data);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Node* addNode(T const& data)
            {
                return addNode(Config::template typed_store<T>(data), Config::template typed_typeToValue<T>());
            }
            
            // By default edges point from 0-index to all others
            inline Edge* addEdge(CoreData const& data, TypeId type, std::vector<Node*> const& nodes, bool invert = false)
            {
                auto ret = Base::addEdge(data, nodes, invert);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Edge* addEdge(T const& data, std::vector<Node*> const& nodes, bool invert = false)
            {
                return addEdge(Config::template typed_store<T>(data), Config::template typed_typeToValue<T>(), nodes, invert);
            }
            
            inline Prop* addProp(CoreData const& data, TypeId type, Node* on_node)
            {
                auto ret = Base::addProp(data, on_node);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Prop* addProp(T const& data, Node* on_node)
            {
                return addProp(Config::template typed_store<T>(data), Config::template typed_typeToValue<T>(), on_node);
            }

            inline Prop* addProp(CoreData const& data, TypeId type, Edge* on_edge)
            {
                auto ret = Base::addProp(data, on_edge);
                ret->type = type;

                return ret;
            }

            template<typename T>
            inline Prop* addProp(T const& data, Edge* on_edge)
            {
                return addProp(Config::template typed_store<T>(data), Config::template typed_typeToValue<T>(), on_edge);
            }

        // Getter functions
        public:
            using Base::labelCount;
            using Base::nodeCount;
            using Base::edgeCount;
            using Base::propCount;

            using Base::getNodeLabels;
            using Base::getEdgeNodes;
            using Base::isEdgeInverted;

            TypeId getType(Core* core)
            {
                return core->type;
            }

        // Iteration functions
        public:
            using Base::forAllLabels;
            using Base::forAllNodes;
            using Base::forAllEdges;
            using Base::forAllNodesInLabel;
            using Base::forAllLabelsOnNode;
            using Base::forAllEdgesOnNode;
            using Base::forAllPropsOnNode;

        // Update functions
        public:
            using Base::attachLabel;
            using Base::attachEdge;

            void attachType(Core* core, TypeId type)
            {
                core->type = type;
            }

        // Temporary
        // TODO: Create a graph that uses types to compile time prove properties
        // TODO: move the only functions there
        public:
            template<typename T>
            T const* onlyPropOfTypeOnNode(Node const* node) const
            {
                auto searchType = Config::template typed_typeToValue<T>();
                T const* result = nullptr;

                forAllPropsOnNode(node,
                    [&](auto prop) -> bool
                    {
                        if (prop->type == searchType)
                        {
                            result = Config::template typed_load<T>(prop->data);
                            return false;
                        }
                        return true;
                    });

                return result;
            }
        };
    };
}