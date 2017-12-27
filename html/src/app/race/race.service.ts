import {Injectable} from "@angular/core";
import {Observable} from "rxjs/Observable";
import {Subject} from "rxjs/Subject";
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {HttpClient} from "@angular/common/http";
import {Config} from "../app.config";

export interface Roi {
    x: number;
    y: number;
    width: number;
    height: number;
    type?: string;
    color?: string;
}

export interface CarObserver {
    condition_achieved: boolean;
    type: string;
    active: boolean;
    roi: Roi;
}

export interface ObserverGroup {
    group: ObserverGroup;
    observers: CarObserver[];
}

export interface Race {
    name: string;
    running: boolean;
    selected: boolean;
    group: ObserverGroup;
}

@Injectable()
export class RaceService {
    private race: Subject<any> = new BehaviorSubject(null);

    constructor(private http: HttpClient, private config: Config) {
    }

    getRaces(): Observable<Race[]> {
        return this.http.get(this.config.get('race.all')).map(response => {
            return (response as Race[]);
        });
    }

    loadRace(name: string): Observable<Race> {
        return this.http.get(this.config.get('race.get').replace(':name', name)).map((response: Race) => {
            const result = response;
            this.race.next(result);
            return result;
        });
    }

    selectRace(name: string): Observable<Race> {
        return this.http.post(this.config.get('race.select').replace(':name', name),{}).map((response: Race) => {
            const result = response;
            this.race.next(result);
            return result;
        });
    }

    getRace(): Observable<Race> {
        return this.race;
    }

    saveRace(race: Race): Observable<Race> {
        const org = this.cloneAndStrip(race);
        return this.http.post(this.config.get('race.save').replace(':name', org.name), JSON.stringify(org))
            .map((response: Race) => response);
    }

    private cloneAndStrip(race: Race) {
        const org: Race = JSON.parse(JSON.stringify(race));
        this.stripObserversGroup(org.group);
        return org;
    }

    private stripObserversGroup(group: ObserverGroup) {
        group.observers.forEach(observer => {
            delete observer.roi.type;
            delete observer.roi.color;
        });
        if (group.group) {
            this.stripObserversGroup(group.group);
        }
    }
}