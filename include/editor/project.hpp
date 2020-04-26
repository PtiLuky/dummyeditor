#ifndef EDITORPROJECT_H
#define EDITORPROJECT_H

#include <QDomDocument>
#include <QStandardItem>
#include <QString>

#include "dummyrpg/game.hpp"

namespace Editor {
class MapsTreeModel;

//////////////////////////////////////////////////////////////////////////////
//  Project class
//////////////////////////////////////////////////////////////////////////////
struct tMapInfo
{
    std::string m_mapName;
    std::string m_chispetPath;
    std::string m_musicPath;
    uint16_t m_width  = 0;
    uint16_t m_height = 0;
};

class Project
{
public:
    explicit Project(const QString& folder);

    const QString& projectPath() const;
    const Dummy::GameStatic& game() const;
    MapsTreeModel* mapsModel() const;
    const Dummy::Map* currMap() const;

    // Setters
    void setModified(bool isModified) { m_isModified = isModified; }

    // Utils
    void saveProject();
    void createMap(const tMapInfo& mapInfo, QStandardItem& parent);
    bool loadMap(const QString& mapName);

    static std::shared_ptr<Project> create(const QString& projectRootPath);

private:
    void dumpToXmlNode(QDomDocument& document, QDomElement& xmlNode, const QStandardItem* modelItem);
    void registerMaps(const QDomNode& mapsNode);

private:
    Dummy::GameStatic m_game;
    bool m_isModified = false;

    QString m_projectPath;
    std::unique_ptr<MapsTreeModel> m_mapsModel;
    std::unordered_map<std::string, uint16_t> m_mapNameToId;
    std::shared_ptr<Dummy::Map> m_currMap;
};

} // namespace Editor

#endif // EDITORPROJECT_H
