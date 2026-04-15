#pragma once
#include <queue>
#include <ContainerSequential.h>
#include <ContainerAdapter.h>
#include <string>

namespace NodeType
{
    constexpr char Empty = 'o';
    constexpr char Obstacle = '*';
    constexpr char Checkpoint = '|';
}

namespace nam
{

    struct PursuitPoint {
        XMFLOAT3 position;
        int value;
    };

    struct Compare {
        bool operator()(PursuitPoint a, PursuitPoint b) {
            return a.value > b.value; 
        }
    };

    struct Node
    {
        XMFLOAT3 m_pos = {};

        XMINT3 m_nodePos = {};
        char m_data;
        int m_id = -1;
        bool m_isWalkable = false;
        bool m_isVisited = false;
        bool m_isACheckPoint = false;

        float m_distToStart = INTMAX_MAX;
        float m_distToTarget = 0;

        float m_totalCost = 0;

        std::vector<std::pair<Node*,float>> m_neighbors;
        Node* mp_cameFrom;

        void CalculateTotalCost();
        void ResetData();
    };

    struct CompareCostCells {
        bool operator()(const Node* a, const Node* b) const {
            return a->m_totalCost > b->m_totalCost;
        }
    };

    struct Floor
    {
        Vector<Vector<Node*>> m_floorGrid;
    };

    struct Grid
    {

        Vector<Floor> m_grid;

        static size_t GridIndex(size_t row, size_t col, size_t height, size_t colCount, size_t heightCount);
        static size_t StairIndex(size_t row, size_t col, size_t height, size_t colCount);

        Node* GetNode(size_t row, size_t col, size_t floor);
        Node* GetNearestNode(XMFLOAT3 pos);
        Node* GetNodesFromID(int id);
        Node* GetCheckpoint();

        void InitGrid(Vector<Vector<std::pair<char, XMFLOAT3>>>& FloorGrid);
        void InitFloor(Vector<Vector<Node*>>& gridFloor);
        void InitNeighbor(Floor& floor);
        bool VerifyNode(size_t floor, size_t row, size_t col);

        void UpdateGrid();

        void ResetGrid();
        bool UpdateNodePosition(char c, Node* start, Node* end);

    };
    class PathFinding
    {
    private:

        PrioQueue<Node*,Vector<Node*>, CompareCostCells> m_priorityQueue;
        Vector<Node*> m_path;
        

        void CalCulateDistanceManatthan(Node* start);
        void CalculateTotalCost(Node* node);

        void ClearPriorityQueue();
        void ClearPath();

        void CheckNeighbors(Node * currentNode );
    public:

        Node* m_End;

        PathFinding() = default;
        ~PathFinding();

        void InitEndNode(Node* end);
        void InitStartNode(Node* start);
        bool FindPath(Node* start);
        void BuildPathNodes(Node* startNode);
        Vector<Node*>& GetPathNodes();

    };
}