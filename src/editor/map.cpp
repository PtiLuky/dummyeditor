#include "editor/map.hpp"

#include <QDebug>
#include <filesystem>
#include <fstream>

#include "editor/layerBlocking.hpp"
#include "editor/layerEvents.hpp"
#include "editor/layerGraphic.hpp"
#include "utils/Logger.hpp"


namespace fs = std::filesystem;

namespace Editor {
Map::Map(const std::filesystem::path& project, const std::string& name)
    : Dummy::Core::LuaMap(project, name)
{
    for (auto& floor : m_floors) {
        m_editorFloors.push_back(std::make_unique<Floor>(floor));
    }
}

Map::~Map() {}

void Map::setName(const std::string& newName)
{
    m_name = newName;
}

void Map::setChipset(const std::string& chipset)
{
    m_chipset = chipset;
}

void Map::setMusic(const std::string& music)
{
    m_music = music;
}

void Map::reset(std::uint16_t width, std::uint16_t height)
{
    m_width  = width;
    m_height = height;
}

void Map::save()
{
    // Save the blocking layer, then the graphic info.
    saveBlockingLayers();
    saveGraphicLayers();
    saveEventsFile();
}

void Map::saveEventsFile()
{
    // Simply create the .lua file if it does not exist.
    std::string filename(m_name + ".lua");
    fs::path filePath(m_projectPath / "maps" / filename);
    if (! fs::exists(filePath)) {
        std::ofstream ofs(filePath);
        ofs.close();
    }
}

void Map::resizeGraphicLayer(Editor::GraphicLayer& graphicLayer, std::uint16_t width, std::uint16_t height)
{
    // XXX: fix this
    Dummy::Core::GraphicLayer newGraphicLayer(width, height, {-1, -1});

    for (std::uint16_t y = 0; y < height; ++y) {
        for (std::uint16_t x = 0; x < width; ++x) {
            if (x < m_width && y < m_height) {
                newGraphicLayer.set({x, y}, graphicLayer.at({x, y}));
            } else {
                newGraphicLayer.set({x, y}, {-1, -1});
            }
        }
    }
    graphicLayer.layer() = std::move(newGraphicLayer);
}

void Map::resizeBlockingLayer(Editor::BlockingLayer& blockingLayer, std::uint16_t width, std::uint16_t height)
{
    width *= 2;
    height *= 2;
    Dummy::Core::BlockingLayer newBlockingLayer(width, height, 0);

    for (std::uint16_t y = 0; y < height; ++y) {
        for (std::uint16_t x = 0; x < width; ++x) {
            if (x < m_width && y < m_height) {
                newBlockingLayer.set({x, y}, blockingLayer.at({x, y}));
            } else {
                newBlockingLayer.set({x, y}, 0);
            }
        }
    }
    blockingLayer.layer() = std::move(newBlockingLayer);
}

void Map::resize(std::uint16_t width, std::uint16_t height)
{
    for (const auto& floor : m_editorFloors) {
        resizeFloor(*floor, width, height);
    }
    m_width  = width;
    m_height = height;
}

void Map::resizeFloor(Editor::Floor& floor, std::uint16_t width, std::uint16_t height)
{
    resizeBlockingLayer(floor.blockingLayer(), width, height);

    for (auto& [position, layer] : floor.graphicLayers()) {
        resizeGraphicLayer(*layer, width, height);
    }
}

void Map::saveBlockingLayers()
{
    std::uint32_t magicNumber = BLK_MAGIC_WORD;
    std::string filename(m_name + ".blk");
    std::ofstream ofs(m_projectPath / "maps" / filename, std::ios::binary);

    // Write the magic number
    ofs.write(reinterpret_cast<const char*>(&magicNumber), sizeof(std::uint32_t));

    // Write the blocking layers
    for (auto& floor : m_floors) {
        floor.blockingLayer().writeToStream(ofs);
    }

    ofs.close();
}

void Map::saveGraphicLayers()
{
    std::uint32_t magicNumber = MAP_MAGIC_WORD;
    std::uint16_t version     = 2; // XXX for now.
    std::string filename(m_name + ".map");
    std::ofstream ofs(m_projectPath / "maps" / filename, std::ios::binary);

    // write the magic number
    ofs.write(reinterpret_cast<const char*>(&magicNumber), sizeof(std::uint32_t));

    // write the version number
    ofs.write(reinterpret_cast<const char*>(&version), sizeof(std::uint16_t));

    // write the map's dimensions
    ofs.write(reinterpret_cast<const char*>(&m_width), sizeof(std::uint16_t));
    ofs.write(reinterpret_cast<const char*>(&m_height), sizeof(std::uint16_t));

    // write the floors count
    ofs.write(reinterpret_cast<char*>(&m_floorsCount), sizeof(std::uint8_t));

    // write the chipset
    writeStdString(ofs, m_chipset);

    // write the music
    writeStdString(ofs, m_music);

    // write the floors
    for (const auto& floor : m_floors) {
        writeFloor(ofs, floor);
    }
}

void Map::writeStdString(std::ofstream& ofs, const std::string& str)
{
    std::uint32_t size = static_cast<std::uint32_t>(str.size());
    ofs.write(reinterpret_cast<const char*>(&size), sizeof(std::uint32_t));
    if (size > 0) {
        ofs.write(str.c_str(), static_cast<std::streamsize>(str.size()));
    }
}

void Map::writeFloor(std::ofstream& ofs, const Dummy::Core::Floor& floor) const
{
    // Write the layers count.
    std::uint8_t layersCount = floor.graphicLayers().size();
    ofs.write(reinterpret_cast<char*>(&layersCount), sizeof(std::uint8_t));

    for (const auto& [position, layer] : floor.graphicLayers()) {
        ofs.write(reinterpret_cast<const char*>(&position), sizeof(std::int8_t));
        layer.writeToStream(ofs);
    }
}

void Map::addFloor(std::unique_ptr<Editor::Floor> floor)
{
    m_floors.push_back(floor->localFloor());
    m_editorFloors.push_back(std::move(floor));
    ++m_floorsCount;
}

} // namespace Editor
