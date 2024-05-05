module.exports = (sequelizeConfig, Sequelize) => {
    // Set Model
    const Tutorial = sequelizeConfig.define(
        'tutorial',
        {
            title: {
                type: Sequelize.STRING
            },
            description: {
                type: Sequelize.STRING
            },
            published: {
                type: Sequelize.BOOLEAN
            }
        }
    );
    
    const Test1 = sequelizeConfig.define(
        'test',
        {
            count: {
                type: Sequelize.BIGINT
            },
            disabledType:{
                type: Sequelize.STRING
            }
        }
    );
    //https://pjt3591oo.github.io/sequelizejs_translate/build/html/CoreConcepts/DateTypes.html
    return Test1;
};
