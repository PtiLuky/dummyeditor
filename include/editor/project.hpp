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

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(const QString& folder);

    const QString& projectPath() const;
    const Dummy::GameStatic& game() const;
    Dummy::GameStatic& game();
    MapsTreeModel* mapsModel() const;
    const Dummy::Map* currMap() const;
    Dummy::Map* currMap();
    bool isModified() const;

    void testMap();

    // Utils
    void saveProject();
    bool saveCurrMap();
    void createMap(const tMapInfo& mapInfo, QStandardItem& parent);
    bool loadMap(const QString& mapName);
    bool mapExists(const QString& mapName);
    bool renameCurrMap(const QString& newName);

    void createSprite();
    Dummy::AnimatedSprite* spriteAt(Dummy::sprite_id);

    static QString sanitizeMapName(const QString& unsafeName);

    static std::shared_ptr<Project> create(const QString& projectRootPath);

public slots:
    void changed();

signals:
    void saveStatusChanged(bool isSaved);

private:
    void dumpToXmlNode(QDomDocument& document, QDomElement& xmlNode, const QStandardItem* modelItem);
    void registerMaps(const QDomNode& mapsNode);

private:
    Dummy::GameStatic m_game;
    bool m_isModified = false;

    QString m_projectPath;
    QString m_currMapName;
    std::unique_ptr<MapsTreeModel> m_mapsModel;
    std::unordered_map<std::string, uint16_t> m_mapNameToId;
    std::shared_ptr<Dummy::Map> m_currMap;
};

} // namespace Editor

#endif // EDITORPROJECT_H
