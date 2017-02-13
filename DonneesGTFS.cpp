//
// Created by Mario Marchand on 16-12-29.
//

#include "DonneesGTFS.h"

using namespace std;

//! \brief construit un objet GTFS
//! \param[in] p_date: la date utilisée par le GTFS
//! \param[in] p_now1: l'heure du début de l'intervalle considéré
//! \param[in] p_now2: l'heure de fin de l'intervalle considéré
//! \brief Ces deux heures définissent l'intervalle de temps du GTFS; seuls les moments de [p_now1, p_now2) sont considérés
DonneesGTFS::DonneesGTFS(const Date &p_date, const Heure &p_now1, const Heure &p_now2)
        : m_date(p_date), m_now1(p_now1), m_now2(p_now2), m_nbArrets(0), m_tousLesArretsPresents(false)
{
}

//! \brief partitionne un string en un vecteur de strings
//! \param[in] s: le string à être partitionner
//! \param[in] delim: le caractère utilisé pour le partitionnement
//! \return le vecteur de string sans le caractère utilisé pour le partitionnement
vector<string> DonneesGTFS::string_to_vector(const string &s, char delim)
{
    stringstream ss(s);
    string item;
    vector<string> elems;
    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

//! \brief ajoute les lignes dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les lignes
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterLignes(const std::string &p_nomFichier) {

    try {
        //Ouverture du fichier.
        fstream fichierLigne;
        fichierLigne.open(p_nomFichier);

        if(!fichierLigne.is_open()){
            //throw std::logic_error("Erreur lors de l'ouverture du fichier.");
            }

        else{
            string ligne;
            char delimitateur = ',';
            vector<string> vecteurLigne;

            //J'effectue une getline pour éviter de prendre l'entête du fichier.
            getline(fichierLigne, ligne);

            //Tant qu'il y a des éléments dans le fichier, on créé des lignes.
            while(!fichierLigne.eof()) {
                getline(fichierLigne, ligne);
                if(ligne.empty()){
                    throw logic_error("Erreur lors de la lecture du fichier");
                }

                //J'enlève toutes les guillemets de la string.
                string enleverGuillement = ligne;
                for(int i = 0; i < enleverGuillement.size(); i++){
                    if(enleverGuillement[i] == '"'){
                        enleverGuillement.erase(enleverGuillement.begin()+i);
                    }
                }

                //Je converti ma string pour un vector pour sortir les informations nécessaires.
                //Pour sortir m_ligne.
                vecteurLigne = string_to_vector(enleverGuillement, delimitateur);
                int intConverti;
                string stringAConvertir;
                stringAConvertir = vecteurLigne.at(0);
                intConverti = atoi(stringAConvertir.c_str());
                unsigned int uInt = (unsigned int)intConverti;

                //Pour m_ligne_par_numero.
                string numero_ligne;
                numero_ligne = vecteurLigne.at(2);

                //Pour m_description.
                string description_route;
                description_route = vecteurLigne.at(4);

                //Pour m_categorie.
                string couleur_ligne;
                CategorieBus categorie_ligne;
                couleur_ligne = vecteurLigne.at(7);
                categorie_ligne = Ligne::couleurToCategorie(couleur_ligne);

                //Création de l'objet Ligne.
                Ligne *objetLigne = new Ligne(uInt, numero_ligne, description_route, categorie_ligne);

                //J'ajoute ce nouvel objet dans les maps m_ligne et m_ligne_par_numero.
                m_lignes.insert(std::make_pair(uInt, *objetLigne));
                m_lignes_par_numero.insert(std::make_pair(numero_ligne,*objetLigne));
            }
            fichierLigne.close();
        }
    }
    catch (std::logic_error) {
    }
}

//! \brief ajoute les stations dans l'objet GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterStations(const std::string &p_nomFichier) {
    try {
        fstream fichierStations;
        fichierStations.open(p_nomFichier);

        if (!fichierStations.is_open()) {
            throw std::logic_error("Erreur d'ouverture du fichier.");
        }

        string ligne;
        char delimitateur = ',';
        vector<string> vecteurStation;

        //Je prends la première ligne pour ne pas prendre l'entête.
        getline(fichierStations, ligne);

        //Tant qu'il y a des éléments dans le fichier, on créé des lignes.
        while (getline(fichierStations, ligne)) {

            if(ligne.empty()){
                //throw logic_error("Erreur lors de la lecture du fichier");
                continue;
            }

            //J'enlève toutes les guillemets de la string.
            string enleverGuillement = ligne;
            string guillemetsEnleve;
            for (int i = 0; i < enleverGuillement.size(); i++) {
                if (enleverGuillement[i] != '"') {
                    guillemetsEnleve = guillemetsEnleve + enleverGuillement[i];
                }
            }

            //Maintenant que les guillements sont enlevées, je peux aller chercher les informations pour créer les stations.
            vecteurStation = string_to_vector(guillemetsEnleve, delimitateur);

            //Pour stop_id
            int stop_id;
            string stringAConvertir;
            stringAConvertir = vecteurStation.at(0);
            stop_id = atoi(stringAConvertir.c_str());


            //Pour aller chercher le nom de la station (m_nom)
            string nomStation = vecteurStation.at(1);

            //Pour avoir stop_desc (m_description)
            string descStation = vecteurStation.at(2);

            //pour les coordonnées géographiques.
            string string_latitude = vecteurStation.at(3);
            string string_longitude = vecteurStation.at(4);
            double latitude = atof(string_latitude.c_str());
            double longitude = atof(string_longitude.c_str());

            //Création de l'object coordonnées afin de créer l'objet station.
            Coordonnees *CoordStation = new Coordonnees(latitude,longitude);

            //Création de l'objet station.
            Station *nouvelleStation = new Station((unsigned int) stop_id, nomStation, descStation, *CoordStation);

            //Insertion de la station dans le conteneur m_stations.
            m_stations.insert(std::make_pair((unsigned int) stop_id, *nouvelleStation));

        }
        fichierStations.close();
    }
    catch (std::logic_error &ex) {
    }
}

//! \brief ajoute les transferts dans l'objet GTFS
//! \breif Cette méthode doit âtre utilisée uniquement après que tous les arrêts ont été ajoutés
//! \brief les transferts (entre stations) ajoutés sont uniquement ceux pour lesquelles les stations sont prensentes dans l'objet GTFS
//! \brief les transferts d'une station à elle m^e ne sont pas ajoutés
//! \param[in] p_nomFichier: le nom du fichier contenant les station
//! \throws logic_error si un problème survient avec la lecture du fichier
//! \throws logic_error si tous les arrets de la date et de l'intervalle n'ont pas été ajoutés
void DonneesGTFS::ajouterTransferts(const std::string &p_nomFichier) {
    fstream ficherTransfert;
    ficherTransfert.open(p_nomFichier);

    if(!ficherTransfert.is_open()){
        throw std::logic_error("Erreur ouverture de fichier.");
    }
    string ligne;
    char delimitateur = ',';
    vector<string> vecteurTransfert;

    //Je prends la première ligne pour ne pas prendre l'entête.
    getline(ficherTransfert, ligne);

    //Tant qu'il y a des éléments dans le fichier, on créé des lignes.
    while(getline(ficherTransfert,ligne))
    {
        if(ligne.empty()){
            throw logic_error("Erreur lors de la lecture du fichier");
        }

        vecteurTransfert = string_to_vector(ligne, ',');
        int from_stop_id = atoi(vecteurTransfert.at(0).c_str());
        int to_stop_id = atoi(vecteurTransfert.at(1).c_str());
        int min_transfer_time = atoi(vecteurTransfert.at(3).c_str());
        unsigned int uFrom_stop_id = (unsigned int) from_stop_id;
        unsigned int uTo_stop_id = (unsigned int) to_stop_id;
        unsigned int uMin_transfer_time = (unsigned int) min_transfer_time;

        if(from_stop_id != to_stop_id)
        {
            auto stationProvenance = m_stations.find((unsigned int) from_stop_id);
            auto stationDestination = m_stations.find((unsigned int) to_stop_id);
            if(stationProvenance != m_stations.end() && stationDestination != m_stations.end())
            {
                m_transferts.push_back(make_tuple(uFrom_stop_id, uTo_stop_id, uMin_transfer_time));
            }
        }
    }
}


//! \brief ajoute les services de la date du GTFS (m_date)
//! \param[in] p_nomFichier: le nom du fichier contenant les services
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterServices(const std::string &p_nomFichier) {
    try {
        fstream fichierServices;
        fichierServices.open(p_nomFichier);

        if (!fichierServices.is_open()) {
            throw std::logic_error("Erreur d'ouverture du fichier.");
        }
        string ligne;
        char delimitateur = ',';
        vector<string> vecteurService;

        //Je prends la première ligne pour ne pas prendre l'entête.
        getline(fichierServices, ligne);

        //Tant qu'il y a des éléments dans le fichier, on créé des lignes.
        while (!fichierServices.eof()) {
            getline(fichierServices, ligne);

            if(ligne.empty()){
                throw logic_error("Erreur lors de la lecture du fichier");
            }

            //J'enlève toutes les guillemets de la string.
            string enleverGuillement = ligne;
            string guillemetsEnleve;
            for (int i = 0; i < enleverGuillement.size(); i++) {
                if (enleverGuillement[i] != '"') {
                    guillemetsEnleve = guillemetsEnleve + enleverGuillement[i];
                }
            }

            //Transfert de la ligne dans un vecteur pour aller chercher les infos.
            vecteurService = string_to_vector(guillemetsEnleve, delimitateur);

            //Aller chercher les infos du fichier.
            string idService = vecteurService.at(0);
            string dateService = vecteurService.at(1);

            //Je dois maintenant prendre l'année, le mois et la date dans la string dateService.
            string annee, mois, jour;
            for (int i = 0; i < dateService.size(); i++) {
                if (i <= 3) {
                    annee = annee + dateService[i];
                }
                else if (i >= 4 && i <= 5) {
                    mois = mois + dateService[i];
                }
                else if (i >= 6 && i <= 7) {
                    jour = jour + dateService[i];
                }
            }
            //Modification des string en int puis unsigned int.
            int anneeConv = atoi(annee.c_str());
            int moisConv = atoi(mois.c_str());
            int jourConv = atoi(jour.c_str());
            unsigned int uAnnee = (unsigned int) anneeConv;
            unsigned int uMois = (unsigned int) moisConv;
            unsigned int uJour = (unsigned int) jourConv;

            Date *dateLigne = new Date(uAnnee, uMois, uJour);
            Date dateDeLaLigne = *dateLigne;

            if (m_date == dateDeLaLigne) {
                m_services.insert(idService);
            }
        }
        fichierServices.close();
    }
    catch (exception) {
    }
}

//! \brief ajoute les voyages de la date
//! \brief seuls les voyages dont le service est présent dans l'objet GTFS sont ajoutés
//! \param[in] p_nomFichier: le nom du fichier contenant les voyages
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterVoyagesDeLaDate(const std::string &p_nomFichier) {
    try {
        fstream fichierVoyage;
        fichierVoyage.open(p_nomFichier);

        if (!fichierVoyage.is_open()) {
            //throw std::logic_error("Erreur d'ouverture du fichier.");
        }
        string ligne;
        char delimitateur = ',';
        vector<string> vecteurVoyage;

        //Je prends la première ligne pour ne pas prendre l'entête.
        getline(fichierVoyage, ligne);

        //Tant qu'il y a des éléments dans le fichier, on créé des lignes.
        while (!fichierVoyage.eof()) {
            getline(fichierVoyage, ligne);

            if (ligne.empty()) {
                //throw logic_error("Erreur lors de la lecture du fichier");
                continue;
            }

            string enleverGuillement = ligne;
            string guillemetsEnleve;
            for (int i = 0; i < enleverGuillement.size(); i++) {
                if (enleverGuillement[i] != '"') {
                    guillemetsEnleve = guillemetsEnleve + enleverGuillement[i];
                }
            }

            vecteurVoyage = string_to_vector(guillemetsEnleve, delimitateur);
            //Aller chercher le trip_id dans le vecteur, il se trouve à la 2e position dans le vecteur.
            string trip_id = vecteurVoyage.at(2);

            //Aller chercher le route_id du voyage. Il est à la position 0 du vecteur.
            string routeID = vecteurVoyage.at(0);
            int routeIDint = atoi(routeID.c_str());
            unsigned int IDroute = (unsigned int) routeIDint;

            //Aller chercher le service_id, celui-ci est à la position 1.
            string service_id = vecteurVoyage.at(1);

            //aller chercher le trip_headline. Il est à la position 3 du vecteur.
            string trip_headsign = vecteurVoyage.at(3);


            auto servicePresent = m_services.find(service_id);
            if(servicePresent != m_services.end()){
                //Création de l'objet Voyage et je l'assigne à m_voyage.
                Voyage *voyageACreer = new Voyage(trip_id, IDroute, service_id, trip_headsign);
                m_voyages.insert(std::make_pair(trip_id, *voyageACreer));
            }
        }
        fichierVoyage.close();

    } catch (exception) { }
}

//! \brief ajoute les arrets aux voyages présents dans le GTFS si l'heure du voyage appartient à l'intervalle de temps du GTFS
//! \brief De plus, on enlève les voyages qui n'ont pas d'arrêts dans l'intervalle de temps du GTFS
//! \brief De plus, on enlève les stations qui n'ont pas d'arrets dans l'intervalle de temps du GTFS
//! \param[in] p_nomFichier: le nom du fichier contenant les arrets
//! \post assigne m_tousLesArretsPresents à true
//! \throws logic_error si un problème survient avec la lecture du fichier
void DonneesGTFS::ajouterArretsDesVoyagesDeLaDate(const std::string &p_nomFichier)
{
    try {
        ifstream fichierArret;
        fichierArret.open(p_nomFichier);

        if (!fichierArret.is_open()) {
            throw std::logic_error("Erreur d'ouverture du fichier.");
        }

        string ligne;
        vector<string> vecteurArret;

        //Je prends la première ligne pour ne pas prendre l'entête.
        getline(fichierArret, ligne);

        while(getline(fichierArret, ligne)) {
            if (!ligne.empty()) {
                vecteurArret = string_to_vector(ligne, ',');

                //Aller chercher le stop_id, ensuite je vérifie si cet arrêt est présent dans m_station.
                string voyage_id = vecteurArret.at(0);
                auto voyagePresent = m_voyages.find(voyage_id);
                if (voyagePresent != m_voyages.end()) {

                    //aller chercher l'heure d'arrivée (m_heure_arrivee).
                    string date_arrivee = vecteurArret.at(1);
                    vector<string> heureAConvertir = string_to_vector(date_arrivee, ':');

                    //Je vais chercher les heures, minutes et secondes puis je les convertie
                    string strHeure = heureAConvertir.at(0);
                    string strMinutes = heureAConvertir.at(1);
                    string strSecondes = heureAConvertir.at(2);
                    int intHeure, intMinutes, intSecondes;
                    intHeure = atoi(strHeure.c_str());
                    intMinutes = atoi(strMinutes.c_str());
                    intSecondes = atoi(strSecondes.c_str());
                    Heure *heure_arrivee = new Heure((unsigned int) intHeure, (unsigned int) intMinutes,
                                                     (unsigned int) intSecondes);

                    //Je répête pour l'heure de départ.
                    string date_depart = vecteurArret.at(2);
                    vector<string> heureDepartAConvertir = string_to_vector(date_depart, ':');

                    string strDepartHeure = heureDepartAConvertir.at(0);
                    string strDepartMinutes = heureDepartAConvertir.at(1);
                    string strDepartSecondes = heureDepartAConvertir.at(2);
                    int intDepartHeure, intDepartMinutes, intDepartSecondes;
                    intDepartHeure = atoi(strDepartHeure.c_str());
                    intDepartMinutes = atoi(strDepartMinutes.c_str());
                    intDepartSecondes = atoi(strDepartSecondes.c_str());
                    Heure *heure_depart = new Heure((unsigned int) intDepartHeure, (unsigned int) intDepartMinutes,
                                                    (unsigned int) intDepartSecondes);

                    auto serviceDeLaJournee = m_services.find((*voyagePresent).second.getServiceId());
                    if (serviceDeLaJournee != m_services.end()) {

                        //Je vérifie que l'heure de départ et d'arrivée concordent avec m_now1 et m_now2.
                        if ((*heure_depart > getTempsDebut() && *heure_arrivee < getTempsFin()) ||
                            (*heure_depart == getTempsDebut())) {

                            //Je vais chercher stop_sequence (m_numero_sequence).
                            string stringStopSequence = vecteurArret.at(4);
                            int numero_sequence = atoi(stringStopSequence.c_str());

                            //Je vais chercher station_id (m_station_id).
                            string stop_id_string = vecteurArret.at(3);
                            int station_id = atoi(stop_id_string.c_str());

                            m_voyages[voyage_id].ajouterArret(Arret::Ptr(
                                    new Arret((unsigned int) station_id, *heure_arrivee, *heure_depart,
                                              (unsigned int) numero_sequence, voyage_id)));
                        }
                    }
                }
            }
        }
        //Je ferme le fichier.
        fichierArret.close();

        //Je supprime les voyages qui ne contiennent pas d'arrêts.
        for (auto iter = m_voyages.begin(); iter != m_voyages.end();) {
            if ((*iter).second.getNbArrets() == 0) {
                //je supprimes le voyage.
                m_voyages.erase(iter++);
            }
            else {
                ++iter;
            }
        }
        //J'ajoute les arrêts de m_voyages dans la station de m_stations,
        for (auto iter = m_voyages.begin(); iter != m_voyages.end(); ++iter) {
            if ((*iter).second.getNbArrets() > 0) {

                //Si le voyage a plus d'un arrêt, je recherche le stop_id de l'arret.
                for (auto &a: (iter->second).getArrets()) {
                    auto station = m_stations.find((*a).getStationId());
                    if (station != m_stations.end()) {
                        //Si le station_id d'arrêt et de Station sont identiques, j'ajoute l'arrêt dans m_station.
                        if ((*station).second.getId() == (*a).getStationId()) {
                            m_stations[(*a).getStationId()].addArret(Arret::Ptr(
                                    new Arret(*a)));
                            m_nbArrets++;
                        }
                    }
                }
            }
        }
        //je supprime mes station qui n'ont pas d'arrêt.
        for (auto station = m_stations.begin(); station != m_stations.end();){
            if((*station).second.getNbArrets() == 0){
                m_stations.erase(station++);
            }
            else {
                ++station;
            }
        }
    }
    catch (exception){}
}


unsigned int DonneesGTFS::getNbArrets() const
{
    return m_nbArrets;
}

size_t DonneesGTFS::getNbLignes() const
{
    return m_lignes.size();
}

size_t DonneesGTFS::getNbStations() const
{
    return m_stations.size();
}

size_t DonneesGTFS::getNbTransferts() const
{
    return m_transferts.size();
}

size_t DonneesGTFS::getNbServices() const
{
    return m_services.size();
}

size_t DonneesGTFS::getNbVoyages() const
{
    return m_voyages.size();
}

void DonneesGTFS::afficherLignes() const
{
    std::cout << "======================" << std::endl;
    std::cout << "   LIGNES GTFS   " << std::endl;
    std::cout << "   COMPTE = " << m_lignes.size() << "   " << std::endl;
    std::cout << "======================" << std::endl;
    for (auto itr = m_lignes_par_numero.begin(); itr != m_lignes_par_numero.end(); ++itr)
    {
        cout << itr->second;
    }
    std::cout << std::endl;
}

void DonneesGTFS::afficherStations() const
{
    std::cout << "========================" << std::endl;
    std::cout << "   STATIONS GTFS   " << std::endl;
    std::cout << "   COMPTE = " << m_stations.size() << "   " << std::endl;
    std::cout << "========================" << std::endl;
    for (auto itr = m_stations.begin(); itr != m_stations.end(); ++itr)
    {
        std::cout << itr->second << endl;
    }
    std::cout << std::endl;
}

void DonneesGTFS::afficherTransferts() const
{
    std::cout << "========================" << std::endl;
    std::cout << "   TRANSFERTS GTFS   " << std::endl;
    std::cout << "   COMPTE = " << m_transferts.size() << "   " << std::endl;
    std::cout << "========================" << std::endl;
    for (unsigned int i = 0; i < m_transferts.size(); ++i)
    {
        std::cout << "De la station " << get<0>(m_transferts.at(i)) << " vers la station " << get<1>(m_transferts.at(i))
                  <<
                  " en " << get<2>(m_transferts.at(i)) << " secondes" << endl;

    }
    std::cout << std::endl;

}


void DonneesGTFS::afficherArretsParVoyages() const
{
    std::cout << "=====================================" << std::endl;
    std::cout << "   VOYAGES DE LA JOURNÉE DU " << m_date << std::endl;
    std::cout << "   " << m_now1 << " - " << m_now2 << std::endl;
    std::cout << "   COMPTE = " << m_voyages.size() << "   " << std::endl;
    std::cout << "=====================================" << std::endl;

    for (auto v_itr = m_voyages.begin(); v_itr != m_voyages.end(); ++v_itr)
    {
        unsigned int ligne_id = (v_itr->second).getLigne();
        auto l_itr = m_lignes.find(ligne_id);
        cout << (l_itr->second).getNumero() << " ";
        cout << v_itr->second << endl;
        for (auto &a: (v_itr->second).getArrets())
        {
            unsigned int station_id = a->getStationId();
            auto s_itr = m_stations.find(station_id);
            std::cout << a->getHeureArrivee() << " station " << s_itr->second << endl;
        }
    }
    std::cout << std::endl;
}

void DonneesGTFS::afficherArretsParStations() const
{
    std::cout << "========================" << std::endl;
    std::cout << "   ARRETS PAR STATIONS   " << std::endl;
    std::cout << "   Nombre d'arrêts = " << m_nbArrets << std::endl;
    std::cout << "========================" << std::endl;
    for (auto s_itr = m_stations.begin(); s_itr != m_stations.end(); ++s_itr)
    {
        std::cout << "Station " << s_itr->second << endl;
        for (auto &a: (s_itr->second).getArrets())
        {
            string voyage_id = (a.second)->getVoyageId();
            auto v_itr = m_voyages.find(voyage_id);
            unsigned int ligne_id = (v_itr->second).getLigne();
            auto l_itr = m_lignes.find(ligne_id);
            std::cout << a.first << " - " << (l_itr->second).getNumero() << " " << v_itr->second << std::endl;
        }
    }
    std::cout << std::endl;
}

const std::map<std::string, Voyage> &DonneesGTFS::getVoyages() const
{
    return m_voyages;
}

const std::map<unsigned int, Station> &DonneesGTFS::getStations() const
{
    return m_stations;
}

const std::vector<std::tuple<unsigned int, unsigned int, unsigned int> > &DonneesGTFS::getTransferts() const
{
    return m_transferts;
}

Heure DonneesGTFS::getTempsFin() const
{
    return m_now2;
}

Heure DonneesGTFS::getTempsDebut() const
{
    return m_now1;
}

const std::unordered_map<unsigned int, Ligne> &DonneesGTFS::getLignes() const
{
    return m_lignes;
}


