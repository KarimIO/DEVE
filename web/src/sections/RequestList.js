import React, { Component } from 'react';

export default class RequestList extends Component {
	render() {
		let empty = this.props.requests.length === 0;
		return (
			<section className={`main-section${this.props.loading ? " loading" : ""}${empty ? " empty":""}`}>
                <div className="gallery">
                    {this.props.requests.map((e, i) => {
                        return <div key={i} style={{backgroundImage: "url(data:image/jpeg;base64,"+e.thumb+")"}}>
                            <div>
                                <span>
                                    <b>{e.user}</b>
                                    <i>{e.views} view{e.views !== 1 && "s"} requested</i>
									<button onClick={(ev) => this.props.grantViews(e.image, e.user)}>Grant Views</button>
                                </span>
                            </div>
                        </div>;
                    })}
                </div>
			</section>
		);
	}
}