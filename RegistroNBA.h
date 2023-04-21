struct RegistroNBA
{
    char home_team[4]{0}; // home team short
    long matchup_id; // this is pk!
    long home_points;
    char away_team[4]{0}; // away team short
    long away_points;
    // other fields ignored because they cluttered stuff
};