#include "pch.h"
#include "PathFinding.h"
namespace nam
{

    void Node::CalculateTotalCost()
    {
        m_totalCost = m_distToStart + m_distToTarget;
    }

    void Node::ResetData()
    {
        if (m_data != NodeType::Empty)
            return;
        mp_cameFrom = nullptr;
        m_distToStart = INTMAX_MAX;
        m_distToTarget = 0;
        m_totalCost = 0;
        m_isVisited = false;
    }

    size_t Grid::GridIndex(size_t row, size_t col, size_t Height, size_t colCount, size_t HeightCount)
    {
        size_t index = row * colCount + col * HeightCount + Height;
        return index;
    }

    size_t Grid::StairIndex(size_t row, size_t col, size_t Height, size_t colCount)
    {
        size_t index = row * colCount + col + Height;
        return index;
    }


    Node* Grid::GetNode(size_t floor,size_t row, size_t col)
    {
        if (VerifyNode(floor, row, col))
            return m_grid[floor].m_floorGrid[row][col];
        else
            return nullptr;
    }

    Node* Grid::GetNearestNode(XMFLOAT3 pos)
    {
        float smallestSquaredDist = (float)INT_MAX;
        Node* nearest = nullptr;

        for (Floor& f : m_grid)
        {
            for (Vector<Node*>& v : f.m_floorGrid)
            {
                for (Node* n : v)
                {
                    if (!n->m_isWalkable)
                        continue;

                    float dx = abs(n->m_pos.x - pos.x);
                    float dz = abs(n->m_pos.z - pos.z);

                    float squaredDist = dx * dx + dz * dz;

                    if (squaredDist < smallestSquaredDist)
                    {
                        nearest = n;
                        smallestSquaredDist = squaredDist;
                    }
                        
                }
            }
        }

        return nearest;
    }


    Node* Grid::GetNodesFromID(int id)
    {
        for (Floor& f : m_grid)
        {
            for (Vector<Node*>& v : f.m_floorGrid)
            {
                for (Node* n : v)
                {
                    if (n->m_id == id)
                        return n;
                }
            }
        }

        return nullptr;
    }

    Node* Grid::GetCheckpoint()
    {
        Vector<Node*> Checkpoints;

        for (Floor& f : m_grid)
        {
            for (Vector<Node*>& v : f.m_floorGrid)
            {
                for (Node* n : v)
                {
                    if (!n->m_isACheckPoint)
                        continue;
                    else if (n->m_isVisited)
                        continue;
                    else
                        Checkpoints.push_back(n);
                   
                }
            }
        }

        if (Checkpoints.empty())
            return nullptr;

        int index = Rng::Int(0, (int)Checkpoints.size() - 1);
        return Checkpoints[index];
    }

    void Grid::InitGrid(Vector<Vector<std::pair<char, XMFLOAT3>>>& FloorGrid)
    {
        Vector<Vector<Node*>> floor;
 
        for (int i = 0; i < FloorGrid.size(); i++)
        {
            Vector<Node*> row;
            for (int j = 0; j < FloorGrid[i].size(); j++)
            {
                Node* n = new Node;
                n->m_isWalkable = true;
                n->m_data = NodeType::Empty;
                if (FloorGrid[i][j].first == NodeType::Checkpoint)
                    n->m_isACheckPoint = true;
                else if (FloorGrid[i][j].first == NodeType::Obstacle)
                    n->m_isWalkable = false;
                else if (FloorGrid[i][j].first == NodeType::Empty)
                    n->m_isVisited = false;

                n->m_pos = FloorGrid[i][j].second;
                n->m_nodePos = { i, (int)m_grid.size(), j };
                row.push_back(n);
            }
            floor.push_back(row);
        }

        InitFloor(floor);
    }

    void Grid::InitFloor(Vector<Vector<Node*>>& gridFloor)
    {
        if (gridFloor.empty())
            return;

        m_grid.push_back(Floor());

        Floor& floor = m_grid.back();
        floor.m_floorGrid = gridFloor;

        InitNeighbor(floor);
    }

    void Grid::InitNeighbor(Floor& floor)
    {
        for (size_t i = 0; i < floor.m_floorGrid.size(); i++)
        {
            for (size_t j = 0; j < floor.m_floorGrid[i].size(); j++)
            {

                size_t floorNumber = m_grid.size() - 1;

                if (!VerifyNode(floorNumber, i - 1, j))
                    continue;


                Node* currentCoords = floor.m_floorGrid[i][j];

                if (!currentCoords->m_isWalkable)
                    continue;

                static const XMINT2 neighborCoords[8] =
                {
                    { -1, -1 }, { 0, -1 }, { 1, -1 },
                    { -1,  0 },            { 1,  0 },
                    { -1,  1 }, { 0,  1 }, { 1,  1 }
                };

                for (int k = 0; k < 8; ++k)
                {
                    int dl = neighborCoords[k].x;
                    int dc = neighborCoords[k].y;

                    int newL = i + dl;
                    int newC = j + dc;


                    if (VerifyNode(floorNumber, newL, newC) == false)
                        continue;

                    Node* pCurrentNeighbor = floor.m_floorGrid[newL][newC];

                    if (!pCurrentNeighbor->m_isWalkable)
                        continue;

                    float cost = 1.f;

                    if (dl != 0 && dc != 0)
                    {
                        int sideX = newL;
                        int sideY = j;

                        int upX = i;
                        int upY = newC;

                        if (!floor.m_floorGrid[sideX][sideY]->m_isWalkable)
                            continue;
                        if (!floor.m_floorGrid[upX][upY]->m_isWalkable)
                            continue;
                        cost = 1.41421356f; // V2 // 1.41421356f
                    }

                        currentCoords->m_neighbors.push_back({ pCurrentNeighbor,cost });

                    size_t floorNumber = m_grid.size() - 1;

                }
            }
        }
    }

    bool Grid::VerifyNode(size_t floor, size_t row, size_t col)
    {
        if (row < 0 || row > m_grid[floor].m_floorGrid.size() -1)
        {
            return false;
        }
        else if (col < 0 || col > m_grid[floor].m_floorGrid[row].size()-1)
        {
            return false;
        }
        else
            return true;
    }

    void Grid::UpdateGrid()
    {
        for (Floor& f : m_grid)
        {
            for (Vector<Node*>& v : f.m_floorGrid)
            {
                for (Node* n : v)
                {
                    if (n->m_data == NodeType::Empty)

                        n->m_isVisited = false;

                    n->mp_cameFrom = nullptr;
                }
            }
        }
    }

    void Grid::ResetGrid()
    {
        for (Floor& f : m_grid)
        {
            for (Vector<Node*>& v : f.m_floorGrid)
            {
                for (Node* n : v)
                {
                    n->ResetData();
                }
            }
        }
    }

    bool Grid::UpdateNodePosition(char c, Node* start, Node* end)
    {
        if (end->m_data != NodeType::Empty)
                return false;
        
        start->m_data = NodeType::Empty;
        start->m_isVisited = false;

        end->m_data = c;
        end->m_isVisited = true;

        end->m_id = -1;
        end->m_data = start->m_data;
        return true;
    }


    void PathFinding::CalCulateDistanceManatthan(Node* start)
    {
        start->m_distToTarget = std::abs(start->m_nodePos.x - m_End->m_nodePos.x)
            + std::abs(start->m_nodePos.z - m_End->m_nodePos.z)
            + std::abs(start->m_nodePos.y - m_End->m_nodePos.y);
    }


    void PathFinding::CalculateTotalCost(Node* node)
    {
        node->m_totalCost = node->m_distToStart + node->m_distToTarget;
    }

    void PathFinding::ClearPriorityQueue()
    {
        PrioQueue<Node*, Vector<Node*>, CompareCostCells> empty;
        std::swap(m_priorityQueue, empty);
    }

    void PathFinding::ClearPath()
    {
        Vector<Node*> empty;
        std::swap(m_path, empty);
    }

    void PathFinding::CheckNeighbors(Node* currentNode)
    {

        for (auto& n : currentNode->m_neighbors)
        {
            if (!n.first->m_isWalkable)
                continue;
            else if (n.first->m_isVisited)
                continue;
            
            float minimalDist = currentNode->m_distToStart + n.second; 

            if (minimalDist < n.first->m_distToStart)
            {
                n.first->mp_cameFrom = currentNode;
                n.first->m_distToStart = currentNode->m_distToStart + 1.f;
                CalCulateDistanceManatthan(n.first);
                n.first->CalculateTotalCost();
                m_priorityQueue.push(n.first);
                
            }
        }
    }



    PathFinding::~PathFinding()
    {
        ClearPriorityQueue();
        ClearPath();

    }

    void PathFinding::InitEndNode(Node* end)
    {
        
        m_End = end;
    }

    void PathFinding::InitStartNode(Node* start)
    {
        start->mp_cameFrom = nullptr;
        start->m_distToStart = 0;

        if (m_End == nullptr)
            return;

        CalCulateDistanceManatthan(start);
        CalculateTotalCost(start);

        m_priorityQueue.push(start);
    }

    bool PathFinding::FindPath( Node* start)
    {
        if (start == nullptr)
            return false;

        ClearPriorityQueue();
        ClearPath();

        InitStartNode(start);

        while (!m_priorityQueue.empty()) {

            Node* cur = m_priorityQueue.top(); 
            m_priorityQueue.pop();

            if (cur->m_isVisited) 
                continue;

            cur->m_isVisited = true;

            if (cur == m_End) {
                BuildPathNodes(start);

                return true;
            }

            CheckNeighbors(cur);
        }
        return false;
    }

    void PathFinding::BuildPathNodes(Node* startNode)
    {
        Node* cur = m_End;
        while ( cur != startNode)
        {
            m_path.push_back(cur);
            cur = cur->mp_cameFrom;
        }
    }

    Vector<Node*>& PathFinding::GetPathNodes()
    {
        return m_path;
    }


}
