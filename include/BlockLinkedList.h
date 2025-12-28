#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>

const int blocksize = 256;

struct Block
{
    std::string index[blocksize];
    int value[blocksize];
};

template<int strlength>
class BlockLinkedList
{
  private:
    std::fstream file;
    std::string file_name;
    std::string file_name_;
    int head;
    int tail;
    int countBlock; // 文件中存了几个Block
    std::vector<int> deletedBlock; // 记录被删除的Block,用于空间回收
    int sizeofBlock = (4 + strlength + 1) * blocksize;
    std::vector<std::string> blockmax_index;
    std::vector<int> blockmax_value;
    std::vector<int> next;
    std::vector<int> last;
    std::vector<int> count;
    
    void writeBlock(Block &block, int &index)
    {
        file.clear();
        file.seekp(index * sizeofBlock);
        char temp[strlength + 1];
        for(int i = 0; i < blocksize; i++)
        {
            memset(temp, 0, strlength + 1);
            if(!block.index[i].empty())
                strncpy(temp, block.index[i].c_str(), strlength);
            temp[strlength] = '\0';
            file.write(temp, strlength + 1);
        }
        for(int i = 0; i < blocksize; i++)
            file.write(reinterpret_cast<char*>(&block.value[i]), 4);
    }

    Block getBlock(int &index)
    {
        file.clear();
        Block block;
        file.seekg(index * sizeofBlock);
        char temp[strlength + 1];
        for(int i = 0; i < blocksize; i++)
        {
            file.read(temp, strlength + 1);
            block.index[i] = std::string(temp);
        }
        for(int i = 0; i < blocksize; i++)
            file.read(reinterpret_cast<char*>(&block.value[i]), 4);
        return block;
    }




    // 分配新块(将block加在index后)
    // index为-1时表示blocklinkedlist为空
    void addBlock(Block &block, int index, int new_index)
    {
        if(index == -1)
            head = tail = new_index;
        else if(index == tail)
        {
            next[index] = new_index;
            last[new_index] = index;
            tail = new_index;
        }
        else
        {
            last[next[index]] = new_index;
            next[new_index] = next[index];
            next[index] = new_index;
            last[new_index] = index;
        }
        writeBlock(block, new_index);
    }

    // 删除块
    void deleteBlock(int index)
    {
        deletedBlock.push_back(index);
        if(index == head)
        {
            if(index == tail)
                head = tail = -1;
            else
            {
                last[next[index]] = -1;
                head = next[head];
            }
        }
        else
        {
            if(index == tail)
            {
                next[last[index]] = -1;
                tail = last[tail];
            }
            else
            {
                last[next[index]] = last[index];
                next[last[index]] = next[index];
            }
        }
    }

    // 合并块(index和index的下一个)
    void mergeBlock(int index)
    {
        if(next[index] == -1)
        {
            file.clear();
            return ;
        }
        if(count[index] + count[next[index]] > blocksize * 2 / 3)
            return ;
        Block block1{getBlock(index)}, block2{getBlock(next[index])};
        for(int i = 0; i < count[next[index]]; i++)
        {
            block1.index[count[index] + i] = block2.index[i];
            block1.value[count[index] + i] = block2.value[i];
        }
        count[index] += count[next[index]];
        blockmax_index[index] = blockmax_index[next[index]];
        blockmax_value[index] = blockmax_value[next[index]];
        writeBlock(block1, index);
        deleteBlock(next[index]);
    }

    // 分裂块
    void divideBlock(int index)
    {
        if(count[index] < blocksize) return ;
        Block block1{getBlock(index)}, block2;
        int start = count[index] - count[index] / 2;
        for(int i = start; i < count[index]; i++)
        {
            block2.value[i - start] = block1.value[i];
            block2.index[i - start] = block1.index[i];
        }
        int new_index;
        if(deletedBlock.empty())
        {
            new_index = countBlock;
            countBlock++;

            blockmax_index.push_back(blockmax_index[index]);
            blockmax_value.push_back(blockmax_value[index]);
            last.push_back(-1);
            next.push_back(-1);
            count.push_back(count[index] / 2);
            count[index] = start;
        }
        else
        {
            new_index = deletedBlock.back();
            deletedBlock.pop_back();

            count[new_index] = count[index] / 2;
            count[index] = start;
            blockmax_index[new_index] = blockmax_index[index];
            blockmax_value[new_index] = blockmax_value[index];
        }
        blockmax_index[index] = block1.index[start - 1];
        blockmax_value[index] = block1.value[start - 1];
        writeBlock(block1, index);
        addBlock(block2, index, new_index);
    }

    void printblock(int index)
    {
        Block block = getBlock(index);
        std::cout << "max:  "  << blockmax_index[index] << ' ' << blockmax_value[index] << '\n';
        std::cout << "count: " << count[index] << '\n';
        for(int i = 0; i < count[index]; i++)
            std::cout << block.index[i] << ' ' << block.value[i] << '\n';
        std::cout << "last: " << last[index] << "   " << "next: " << next[index] << '\n';
        std::cout << std::endl;
    }

  public:
    BlockLinkedList(std::string blocklinkedlist, std::string basicinformation)
    {
        file_name = blocklinkedlist;
        file_name_ = basicinformation;
        file.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
        if(!file)
        {
            file.open(file_name_, std::ios::out | std::ios::binary);
            file.close();
            head = -1;
            tail = -1;
            countBlock = 0;
            deletedBlock.clear();
            blockmax_index.clear();
            blockmax_value.clear();
            next.clear();
            last.clear();
            count.clear();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
            if(!file)
            {
                file.open(file_name, std::ios::out | std::ios::binary);
                file.close();
                file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
            }
        }
        else
        {
            char str[strlength + 1];
            int temp, count_deletedBlock;
            file.seekg(0);
            file.read(reinterpret_cast<char*>(&head), 4);
            file.read(reinterpret_cast<char*>(&tail), 4);
            file.read(reinterpret_cast<char*>(&countBlock), 4);
            file.read(reinterpret_cast<char*>(&count_deletedBlock), 4);
            deletedBlock.clear();
            for(int i = 0; i < count_deletedBlock; i++)
            {
                file.read(reinterpret_cast<char*>(&temp), 4);
                deletedBlock.push_back(temp);
            }
            blockmax_index.clear();
            for(int i = 0; i < countBlock; i++)
            {
                file.read(str, strlength + 1);
                blockmax_index.push_back(std::string(str));
            }
            blockmax_value.clear();
            for(int i = 0; i < countBlock; i++)
            {
                file.read(reinterpret_cast<char*>(&temp), 4);
                blockmax_value.push_back(temp);
            }
            next.clear();
            for(int i = 0; i < countBlock; i++)
            {
                file.read(reinterpret_cast<char*>(&temp), 4);
                next.push_back(temp);
            }
            last.clear();
            for(int i = 0; i < countBlock; i++)
            {
                file.read(reinterpret_cast<char*>(&temp), 4);
                last.push_back(temp);
            }
            count.clear();
            for(int i = 0; i < countBlock; i++)
            {
                file.read(reinterpret_cast<char*>(&temp), 4);
                count.push_back(temp);
            }
            file.close();
            file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
            if(!file)
            {
                file.open(file_name, std::ios::out | std::ios::binary);
                file.close();
                file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
            }
        }
    }

    ~BlockLinkedList()
    {
        file.close();
        file.open(file_name_, std::ios::out | std::ios::binary);
        file.seekp(0);
        file.write(reinterpret_cast<char*>(&head), 4);
        file.write(reinterpret_cast<char*>(&tail), 4);
        file.write(reinterpret_cast<char*>(&countBlock), 4);
        int count_deletedBlock = deletedBlock.size();
        file.write(reinterpret_cast<char*>(&count_deletedBlock), 4);
        for(int i = 0; i < count_deletedBlock; i++)
            file.write(reinterpret_cast<char*>(&deletedBlock[i]), 4);
        char temp[strlength + 1];
        for(int i = 0; i < countBlock; i++)
        {
            memset(temp, 0, strlength + 1);
            strncpy(temp, blockmax_index[i].c_str(), strlength);
            temp[strlength] = '\0';
            file.write(temp, strlength + 1);
        }
        for(int i = 0; i < countBlock; i++)
            file.write(reinterpret_cast<char*>(&blockmax_value[i]), 4);
        for(int i = 0; i < countBlock; i++)
            file.write(reinterpret_cast<char*>(&next[i]), 4);
        for(int i = 0; i < countBlock; i++)
            file.write(reinterpret_cast<char*>(&last[i]), 4);
        for(int i = 0; i < countBlock; i++)
            file.write(reinterpret_cast<char*>(&count[i]), 4);
        file.close();
    }

    void Insert(const std::string &index, int value)
    {
        if(countBlock == 0 || countBlock == deletedBlock.size())
        {
            if(deletedBlock.size() == 0)
            {
                Block newblock;
                newblock.index[0] = index;
                newblock.value[0] = value;
                blockmax_index.push_back(index);
                blockmax_value.push_back(value);
                last.push_back(-1);
                next.push_back(-1);
                count.push_back(1);
                addBlock(newblock, -1, 0);
                countBlock++;
            }
            else
            {
                int Blockindex = deletedBlock.back();
                deletedBlock.pop_back();
                Block newblock;
                newblock.index[0] = index;
                newblock.value[0] = value;
                blockmax_index[Blockindex] = index;
                blockmax_value[Blockindex] = value;
                last[Blockindex] = -1;
                next[Blockindex] = -1;
                count[Blockindex] = 1;
                addBlock(newblock, -1, Blockindex);
            }
        }
        else
        {
            Block block;
            int current = head;
            while(current != -1 && (blockmax_index[current] < index || blockmax_index[current] == index && blockmax_value[current] <  value))
                current = next[current];
            if(current == -1)
            {
                current = tail;
                block = getBlock(current);
                block.index[count[current]] = index;
                block.value[count[current]] = value;
                blockmax_index[current] = index;
                blockmax_value[current] = value;
                count[current]++;
            }
            else
            {
                block = getBlock(current);
                // 找比目标大的最小数
                int l = 0, r = count[current] - 1, mid;
                while(l != r)
                {
                    mid = (l + r) / 2;
                    if(make_pair(block.index[mid], block.value[mid]) < make_pair(index, value))
                        l = mid + 1;
                    else
                        r = mid;
                }
                for(int i = count[current] - 1; i >= l; i--)
                {
                    block.index[i + 1] = block.index[i];
                    block.value[i + 1] = block.value[i];
                }
                block.index[l] = index;
                block.value[l] = value;
                count[current]++;
            }
            writeBlock(block, current);
            divideBlock(current);
        }
    }

    void Delete(const std::string &index, int value)
    {
        int current = head;
        while(current != -1 && (blockmax_index[current] < index || blockmax_index[current] == index && blockmax_value[current] <  value))
            current = next[current];
        if(current == -1) return ;
        Block block{getBlock(current)};
        int l = 0, r = count[current] - 1, mid;
        while(l != r)
        {
            mid = (l + r) / 2;
            if(block.index[mid] == index && block.value[mid] == value)
            {
                if(mid == count[current] - 1)
                {
                    if(count[current] >= 2)
                    {
                        blockmax_index[current] = block.index[mid - 1];
                        blockmax_value[current] = block.value[mid - 1];
                    }
                    else
                    {
                        deleteBlock(current);
                        return ;
                    }
                }
                for(int i = mid + 1; i < count[current]; i++)
                {
                    block.index[i - 1] = block.index[i];
                    block.value[i - 1] = block.value[i];
                }
                count[current]--;
                writeBlock(block, current);
                mergeBlock(current);
                return ;
            }
            if(block.index[mid] < index || block.index[mid] == index && block.value[mid] < value)
                l = mid + 1;
            else
                r = mid;
        }
        if(block.index[l] == index && block.value[l] == value)
        {
            if(l == count[current] - 1)
            {
                if(count[current] >= 2)
                {
                    blockmax_index[current] = block.index[l - 1];
                    blockmax_value[current] = block.value[l - 1];
                }
                else
                {
                    deleteBlock(current);
                    return ;
                }
            }
            for(int i = l + 1; i < count[current]; i++)
            {
                block.index[i - 1] = block.index[i];
                block.value[i - 1] = block.value[i];
            }
            count[current]--;
            writeBlock(block, current);
            mergeBlock(current);
            return ;
        }
    }

    std::vector<int> Find(const std::string &index)
    {
        std::vector<int> answer;
        answer.clear();
        int current = head;
        while(current != -1  && blockmax_index[current] < index)
            current = next[current];
        if(current == -1)
            return answer;
        Block block;
        bool findit = false, findover = false;
        while(current != -1)
        {
            block = getBlock(current);
            int l = -1, r = count[current], mid;
            while(l != r - 1)
            {
                mid = (l + r) / 2;
                if(block.index[mid]  < index)
                    l = mid;
                else
                    r = mid;
            }
            l++;
            while(block.index[l] == index && l < count[current])
            {
                findit = true;
                answer.push_back(block.value[l]);
                l++;
            }
            if(l != count[current] && findit)
                findover = true;
            if(findit == false || findover == true)
                break;
            current = next[current];
        }
        return answer;
    }

    void printblocklinkedlist()
    {
        std::cout << std::endl << "Situation now:" << std::endl;
        if(countBlock == 0 || countBlock == deletedBlock.size())
        {
            std::cout << "The blocklinkedlist is empty" << std::endl;
            std::cout << std::endl;
            return ;
        }
        std::cout << "head: " << head << "  tail: " << tail << std::endl;
        int current = head;
        while(current != -1)
        {
            std::cout << "Block " << current << ": " << std::endl;
            printblock(current);
            current = next[current];
        }
    }
};