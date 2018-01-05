import { Injectable } from "@angular/core";
import { CarObserver, ObserverGroup, Race } from "./race.service";

@Injectable()
export class RaceStripService {

    public stripRace(current: Race): Race {
        let clone: Race = JSON.parse(JSON.stringify(current));
        clone.groups.forEach(group => this.stripGroup(group));
        return clone;
    }

    public stripGroup(group: ObserverGroup): void {
        group.observers.forEach((observer: any) => {
            this.stripObserver(observer);
        });
    }

    public stripObserver(observer: any): any {
        delete observer.roi.type;
        delete observer.roi.color;
        delete observer.error;
        delete observer.angle;
        return observer;
    }

    public clone(obj: any): any {
        return JSON.parse(JSON.stringify(obj));
    }
}