"use strict";

const sqlite3 = require("electron").remote.require("sqlite3");

module.exports = {
    initialize() {
        return new Promise((resolve, reject) => {
            let db = new sqlite3.Database("youtube-cache", (err) => {
                if(err)
                    reject(err);
            });

            let query = `
                CREATE TABLE IF NOT EXISTS searches (
                    search TEXT,
                    response TEXT,
                    PRIMARY KEY (search)
                )
            `;
            db.run(query, (err) => {
                if(err)
                    reject(err);

                resolve(this._new(db));
            });
        });
    },

    _new(db) {
        return {
            set(searchTerm, response) {
                db.run(
                    `
                        INSERT OR REPLACE INTO searches
                        VALUES ($search, $response)
                    `,
                    {
                        $search: searchTerm,
                        $response: JSON.stringify(response)
                    },
                    (err) => {
                        if(err)
                            console.log(err);
                    }
                );
            },

            get(searchTerm) {
                return new Promise((resolve, reject) => {
                    db.get(
                        `
                            SELECT response
                            FROM searches
                            WHERE search = $search
                        `,
                        { $search: searchTerm },
                        (err, row) => {
                            if(err) {
                                console.log(err);
                                reject();
                                return;
                            }

                            if(row)
                                resolve(JSON.parse(row.response));
                            else
                                resolve(null);
                        }
                    );
                });
            }
        };
    }
};
