import React, { Component } from 'react';

export default class MyImages extends Component {
	render() {
		let empty = this.props.gallery.length === 0;
		return (
			<section className={`main-section${this.props.loading ? " loading" : ""}${empty ? " empty":""}`}>
				<div className="gallery">
					{this.props.gallery.map((e, i) => {
						console.log("url(data:image/jpeg;base64,"+e.thumb+")");
						return <div key={i} style={{backgroundImage: "url(data:image/jpeg;base64,"+e.thumb+")"}}>
							<div>
								<span><i>{e.views}</i> view{e.views !== 1 && "s"}</span>
							</div>
						</div>;
					})}
				</div>
			</section>
		);
	}
}