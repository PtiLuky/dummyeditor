#include "editor/project.hpp"

#include <QDir>
#include <QProcess>
#include <algorithm>
#include <fstream>

#include "dummyrpg/serialize.hpp"
#include "utils/logger.hpp"
#include "widgetsMap/mapsTree.hpp"

using std::make_shared;
using std::make_unique;

static const char* const PROJECT_FILE_NAME = "dummy-rpg-project.xml";
static const char* const DATA_FILE_NAME    = "game-data.gdummy";

namespace Editor {

Project::Project(const QString& projectFile)
{
    // Try to read the "project.xml" file that should be present in folderPath.
    QFileInfo fileInfo(projectFile);
    if (fileInfo.isDir())
        fileInfo = QFileInfo(projectFile + "/" + PROJECT_FILE_NAME);

    QFile xmlProjectFile(projectFile);
    QDomDocument projectDom;
    projectDom.setContent(&xmlProjectFile);

    QDomNodeList mapsNodes = projectDom.documentElement().elementsByTagName("maps");
    std::unique_ptr<MapsTreeModel> mapsTree;

    if (mapsNodes.length() > 0) {
        auto mapsNode = mapsNodes.at(0);
        mapsTree      = make_unique<MapsTreeModel>(mapsNode);
    } else {
        QDomNode fakeMapsList;
        mapsTree = make_unique<MapsTreeModel>(fakeMapsList);
    }


    Dummy::GameStatic newGameData(fileInfo.path().toStdString());
    std::ifstream gameDataFile((fileInfo.path() + "/" + DATA_FILE_NAME).toStdString(), std::ios::binary);
    if (gameDataFile.good()) {
        bool bRes = Dummy::Serializer::parseGameFromFile(gameDataFile, newGameData);
        if (! bRes)
            Log::error("Error while loading game data");
    }

    m_mapsModel   = std::move(mapsTree);
    m_game        = std::move(newGameData);
    m_projectPath = fileInfo.path();
}

const QString& Project::projectPath() const
{
    return m_projectPath;
}

const Dummy::GameStatic& Project::game() const
{
    return m_game;
}

Dummy::GameStatic& Project::game()
{
    return m_game;
}

MapsTreeModel* Project::mapsModel() const
{
    return m_mapsModel.get();
}

const Dummy::Map* Project::currMap() const
{
    return m_currMap.get();
}

Dummy::Map* Project::currMap()
{
    return m_currMap.get();
}

bool Project::isModified() const
{
    return m_isModified;
}

void Project::changed()
{
    m_isModified = true;
    emit saveStatusChanged(false);
}

void Project::testMap()
{
    if (m_currMapName.isNull()) {
        Log::error(tr("Impossible to play: no map loaded"));
        return;
    }

    saveProject();
#ifdef _WIN32
    QString playerProgram = "player.exe";
#else
    QString playerProgram = "player";
#endif
    QStringList arguments;
    arguments << m_projectPath << m_currMapName;
    QProcess* myProcess = new QProcess();
    myProcess->start(playerProgram, arguments);
}

std::shared_ptr<Project> Project::create(const QString& projectRootPath)
{
    // Check folder exists and is empty
    const QDir projectDir(projectRootPath);
    if (! projectDir.exists()) {
        Log::error(QObject::tr("Project directory does not exist: %1").arg(projectDir.absolutePath()));
        return nullptr;
    }
    if (! projectDir.isEmpty()) {
        Log::error(QObject::tr("Project directory must be empty: %1").arg(projectDir.absolutePath()));
        return nullptr;
    }

    // Create subfolders
    projectDir.mkdir("maps");
    projectDir.mkdir("images");
    projectDir.mkdir("fonts");
    projectDir.mkdir("sounds");

    // Copy first default tilesheet
    QFile::copy("Resources/images/ClassicRPG_Sheet.png", projectRootPath + "/images/ClassicRPG_Sheet.png");

    // Create DummyRPG Editor project file
    QFile projectFile(projectDir.filePath(PROJECT_FILE_NAME));
    projectFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (! projectFile.isOpen()) {
        Log::error(QObject::tr("Could not create project file: %1").arg(projectFile.fileName()));
        return nullptr;
    }

    return std::make_shared<Project>(projectRootPath);
}

void Project::saveProject()
{
    QDomDocument doc;
    QDomElement projectNode = doc.createElement("project");
    QDomElement mapsNode    = doc.createElement("maps");

    doc.appendChild(projectNode);
    projectNode.appendChild(mapsNode);
    dumpToXmlNode(doc, mapsNode, m_mapsModel->invisibleRootItem());

    QFile file(m_projectPath + "/" + PROJECT_FILE_NAME);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    const int indent = 4;
    doc.save(stream, indent);

    // Save opened map
    bool bRes = saveCurrMap();
    if (bRes) {
        m_isModified = false;
        emit saveStatusChanged(true);
    } else
        Log::error("Error while saving the map...");

    // Save game file
    m_game.cleanupUnused();
    std::ofstream gameDataFile(m_projectPath.toStdString() + "/" + DATA_FILE_NAME, std::ios::binary);
    bRes = Dummy::Serializer::serializeGameToFile(m_game, gameDataFile);
    if (! bRes)
        Log::error("Error while saving the game data...");
}

bool Project::saveCurrMap()
{
    if (m_currMap == nullptr) {
        Log::error("No current map to save");
        return true;
    }

    QString mapPath = m_projectPath + "/maps/" + m_currMapName + MAP_FILE_EXT;
    auto tmp        = mapPath.toStdString();
    std::ofstream mapDataFile(mapPath.toStdString(), std::ios::binary);
    bool bRes = Dummy::Serializer::serializeMapToFile(*m_currMap, mapDataFile);

    return bRes;
}

void Project::dumpToXmlNode(QDomDocument& doc, QDomElement& xmlNode, const QStandardItem* modelItem)
{
    const int nbRows = modelItem->rowCount();
    for (int i = 0; i < nbRows; ++i) {
        const QStandardItem* mapItem = modelItem->child(i);

        QDomElement mapNode = doc.createElement("map");
        mapNode.setAttribute("name", mapItem->text());

        xmlNode.appendChild(mapNode);

        dumpToXmlNode(doc, mapNode, mapItem);
    }
}

void Project::createMap(const tMapInfo& mapInfo, QStandardItem& parent)
{
    if (m_mapsModel == nullptr)
        return;

    if (mapExists(QString::fromStdString(mapInfo.m_mapName)))
        return;

    const uint16_t w            = mapInfo.m_width;
    const uint16_t h            = mapInfo.m_height;
    const QString mapName       = sanitizeMapName(QString::fromStdString(mapInfo.m_mapName));
    const Dummy::char_id chipId = m_game.registerTileset(mapInfo.m_chispetPath);

    if (m_currMap != nullptr)
        saveCurrMap();

    m_currMap     = make_shared<Dummy::Map>(w, h, chipId);
    m_currMapName = mapName;

    // Add the new map into the tree.
    QList<QStandardItem*> mapRow {new QStandardItem(mapName)};
    parent.appendRow(mapRow);
    m_game.registerMap(mapName.toStdString());

    saveProject();
}

bool Project::loadMap(const QString& mapName)
{
    if (mapName == m_currMapName)
        return true;

    if (m_currMap != nullptr)
        saveCurrMap();

    m_currMap     = make_shared<Dummy::Map>();
    m_currMapName = mapName;

    QString mapPath = m_projectPath + "/maps/" + mapName + MAP_FILE_EXT;
    std::ifstream mapDataFile(mapPath.toStdString(), std::ios::binary);
    bool bRes = Dummy::Serializer::parseMapFromFile(mapDataFile, *m_currMap);
    if (! bRes)
        Log::error(QObject::tr("Error while loading the map %1").arg(mapPath));

    return bRes;
}

bool Project::mapExists(const QString& mapName)
{
    std::string strName = mapName.toStdString();
    for (auto& name : m_game.mapNames())
        if (name == strName)
            return true;
    return false;
}

bool Project::renameCurrMap(const QString& newName)
{
    if (mapExists(newName)) {
        Log::error(tr("A map with this name already exists"));
        return false;
    }

    // Change in our id-mapping
    std::string strOldName = m_currMapName.toStdString();
    std::string strNewName = newName.toStdString();
    m_game.renameMap(strOldName, strNewName);

    // Change in file name
    QFile::rename(m_projectPath + "/maps/" + m_currMapName + MAP_FILE_EXT,
                  m_projectPath + "/maps/" + newName + MAP_FILE_EXT);

    // Change in map architecture
    if (m_mapsModel)
        m_mapsModel->renameNode(m_currMapName, newName);

    m_currMapName = newName;
    m_isModified  = true;
    saveProject();

    return true;
}

QString Project::sanitizeMapName(const QString& unsafeName)
{
    QString safeName = unsafeName;
    safeName.remove(QRegularExpression(QString::fromUtf8(
        R"([ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ`~!@#$%\^&*€”+=|:.;<>«»,?/{}'"\\])")));
    if (safeName.isNull())
        safeName = "new_map";
    return safeName;
}

} // namespace Editor
